//
// 由 MightyPork 于 2017/10/15 创建。
//

#include "demo.h"

// 需要这些魔法定义以便我们可以使用 clone()
#define _GNU_SOURCE
#define __USE_GNU

#include <sched.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <malloc.h>
#include <stdlib.h>

volatile int sockfd = -1;
volatile bool conn_disband = false;

TinyFrame *demo_tf;

/**
 * 关闭 socket
 */
void demo_disconn(void)
{
    conn_disband = true;
    if (sockfd >= 0) close(sockfd);
}

/**
 * 演示 WriteImpl - 向对等方发送数据
 *
 * @param buff
 * @param len
 */
void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    printf("\033[32mTF_WriteImpl - 发送帧:\033[0m\n");
    dumpFrame(buff, len);
    usleep(1000);

    if (sockfd != -1) {
        write(sockfd, buff, len);
    }
    else {
        printf("\n没有对等方！\n");
    }
}


/**
 * 客户端后台线程
 *
 * @param unused
 * @return unused
 */
static int demo_client(void *unused)
{
    (void) unused;

    ssize_t n = 0;
    uint8_t recvBuff[1024];
    struct sockaddr_in serv_addr;

    printf("\n--- 启动客户端！ ---\n");

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n 错误：无法创建 socket \n");
        return false;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton 错误发生\n");
        return false;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n 错误：连接失败 \n");
        perror("PERROR ");
        return false;
    }

    printf("\n 子进程 \n");

    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
        printf("\033[36m--- 接收 %ld 字节 ---\033[0m\n", n);
        dumpFrame(recvBuff, (size_t) n);
        TF_Accept(demo_tf, recvBuff, (size_t) n);
    }
    return 0;
}

/**
 * 服务端后台线程
 *
 * @param unused
 * @return unused
 */
static int demo_server(void *unused)
{
    (void) unused;
    ssize_t n;
    int listenfd = 0;
    uint8_t recvBuff[1024];
    struct sockaddr_in serv_addr;
    int option;

    printf("\n--- 启动服务端！ ---\n");

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    option = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof(option));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("绑定失败");
        return 1;
    }

    if (listen(listenfd, 10) < 0) {
        perror("监听失败");
        return 1;
    }

    while (1) {
        printf("\n等待客户端...\n");
        sockfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof(option));
        printf("\n客户端已连接\n");
        conn_disband = false;

        while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0 && !conn_disband) {
            printf("\033[36m--- 接收 %ld 字节 ---\033[0m\n", n);
            dumpFrame(recvBuff, n);
            TF_Accept(demo_tf, recvBuff, (size_t) n);
        }

        if (n < 0) {
            printf("\n 读取错误 \n");
        }

        printf("关闭 socket\n");
        close(sockfd);
        sockfd = -1;
    }
    return 0;
}

/**
 * 信号捕获 - 清理
 *
 * @param sig - 导致此操作的信号
 */
static void signal_handler(int sig)
{
    (void) sig;
    printf("正在关闭...");
    demo_disconn();

    exit(sig); // 传递信号 - 这是不标准的行为，但对调试有用
}

/**
 * 睡美人的最爱函数
 */
void demo_sleep(void)
{
    while (1) usleep(10);
}

/**
 * 启动后台线程
 *
 * 从站首先启动，通常不会初始化事务 - 但它可以
 *
 * @param peer 我们是什么对等方
 */
void demo_init(TF_Peer peer)
{
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    int retc;
    void *stack = malloc(8192);
    if (stack == NULL) {
        perror("糟糕");
        signal_handler(9);
        return;
    }

    printf("正在启动 %s...\n", peer == TF_MASTER ? "主站" : "从站");

    // CLONE_VM    --- 共享堆
    // CLONE_FILES --- 共享 stdout 和 stderr
    if (peer == TF_MASTER) {
        retc = clone(&demo_client, (char *) stack + 8192, CLONE_VM | CLONE_FILES, 0);
    }
    else {
        retc = clone(&demo_server, (char *) stack + 8192, CLONE_VM | CLONE_FILES, 0);
    }

    if (retc == 0) {
        perror("克隆失败");
        signal_handler(9);
        return;
    }

    printf("线程已启动\n");
}
