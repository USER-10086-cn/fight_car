/**
 * @file queue.c
 * @brief 閫氱敤闃熷垪瀹炵幇婧愭枃浠?
 */

#include "queue.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 閾捐〃鑺傜偣缁撴瀯浣?
 */
typedef struct queue_node {
    void* data;                  /**< 鏁版嵁鎸囬拡 */
    struct queue_node* next;     /**< 涓嬩竴涓妭鐐规寚閽?*/
} queue_node_t;

/**
 * @brief 闃熷垪缁撴瀯浣?
 */
struct queue {
    queue_node_t* head;          /**< 闃熷垪澶存寚閽?*/
    queue_node_t* tail;          /**< 闃熷垪灏炬寚閽?*/
    uint16_t count;              /**< 褰撳墠椤圭洰鏁伴噺 */
    uint16_t item_size;          /**< 鍗曚釜椤圭洰鐨勫ぇ灏?*/
};

/**
 * @brief 鍒涘缓闃熷垪
 * 
 * @param item_size 鍗曚釜闃熷垪椤圭洰鐨勫ぇ灏忥紙瀛楄妭锛?
 * @return queue_handle_t 鍒涘缓鐨勯槦鍒楀彞鏌勶紝澶辫触鏃惰繑鍥濶ULL
 */
queue_handle_t queue_create(uint16_t item_size)
{
    queue_handle_t queue;
    if (item_size == 0) {
        return NULL;
    }

    /* 鍒嗛厤闃熷垪鍐呭瓨 */
    queue = (queue_handle_t)malloc(sizeof(struct queue));
    if (queue == NULL) {
        return NULL;
    }

    /* 鍒濆鍖栭槦鍒?*/
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    queue->item_size = item_size;

    return queue;
}

/**
 * @brief 鍒犻櫎闃熷垪骞堕噴鏀捐祫婧?
 * 
 * @param queue 瑕佸垹闄ょ殑闃熷垪鍙ユ焺
 */
void queue_delete(queue_handle_t queue)
{
    queue_node_t* current;
    if (queue == NULL) {
        return;
    }

    /* 閲婃斁鎵€鏈夐摼琛ㄨ妭鐐?*/
    current = queue->head;
    while (current != NULL) {
        queue_node_t* next = current->next;

        /* 閲婃斁鏁版嵁 */
        if (current->data != NULL) {
            free(current->data);
        }

        /* 閲婃斁鑺傜偣 */
        free(current);
        current = next;
    }

    /* 閲婃斁闃熷垪缁撴瀯浣?*/
    free(queue);
}

/**
 * @brief 鍚戦槦鍒楀彂閫侀」鐩?
 * 
 * @param queue 闃熷垪鍙ユ焺
 * @param item 瑕佸彂閫佺殑椤圭洰鎸囬拡
 * @return true 鍙戦€佹垚鍔?
 * @return false 鍙戦€佸け璐?
 */
bool queue_send(queue_handle_t queue, const void* item)
{
    queue_node_t* new_node;
    if (queue == NULL || item == NULL) {
        return false;
    }

    /* 鍒涘缓鏂拌妭鐐?*/
    new_node = (queue_node_t*)malloc(sizeof(queue_node_t));
    if (new_node == NULL) {
        return false;  /* 鍐呭瓨鍒嗛厤澶辫触 */
    }

    /* 鍒嗛厤椤圭洰鏁版嵁鍐呭瓨 */
    new_node->data = malloc(queue->item_size);
    if (new_node->data == NULL) {
        free(new_node);
        return false;  /* 鍐呭瓨鍒嗛厤澶辫触 */
    }

    /* 澶嶅埗椤圭洰鏁版嵁 */
    memcpy(new_node->data, item, queue->item_size);
    new_node->next = NULL;

    /* 娣诲姞鍒伴摼琛ㄥ熬閮?*/
    if (queue->tail == NULL) {
        /* 闃熷垪涓虹┖ */
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    queue->count++;
    return true;
}

/**
 * @brief 浠庨槦鍒楁帴鏀堕」鐩?
 * 
 * @param queue 闃熷垪鍙ユ焺
 * @param item 鐢ㄤ簬瀛樺偍鎺ユ敹椤圭洰鐨勭紦鍐插尯
 * @return true 鎺ユ敹鎴愬姛
 * @return false 鎺ユ敹澶辫触
 */
bool queue_receive(queue_handle_t queue, void* item)
{
    queue_node_t* node;
    if (queue == NULL || item == NULL || queue_is_empty(queue)) {
        return false;
    }

    /* 鑾峰彇澶磋妭鐐?*/
    node = queue->head;

    /* 澶嶅埗椤圭洰鏁版嵁 */
    memcpy(item, node->data, queue->item_size);

    /* 鏇存柊澶存寚閽?*/
    queue->head = node->next;
    if (queue->head == NULL) {
        /* 闃熷垪宸茬┖ */
        queue->tail = NULL;
    }

    /* 閲婃斁椤圭洰鏁版嵁鍜岃妭鐐?*/
    free(node->data);
    free(node);
    queue->count--;

    return true;
}

/**
 * @brief 妫€鏌ラ槦鍒楁槸鍚︿负绌?
 * 
 * @param queue 闃熷垪鍙ユ焺
 * @return true 闃熷垪涓虹┖
 * @return false 闃熷垪涓嶄负绌?
 */
bool queue_is_empty(queue_handle_t queue)
{
    if (queue == NULL) {
        return true;
    }

    return queue->count == 0;
}

/**
 * @brief 鑾峰彇闃熷垪涓綋鍓嶉」鐩暟閲?
 * 
 * @param queue 闃熷垪鍙ユ焺
 * @return uint16_t 闃熷垪涓殑椤圭洰鏁伴噺
 */
uint16_t queue_count(queue_handle_t queue)
{
    if (queue == NULL) {
        return 0;
    }

    return queue->count;
}
