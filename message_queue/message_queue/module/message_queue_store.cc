/**
 * e8yes demo web.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "message_queue/message_queue/module/message_queue_store.h"

namespace e8 {
namespace {

static MessageQueueStore gMessageQueueStore;

} // namespace

MessageQueueStore::MessageQueue::MessageQueue() { sem_init(&queue_resource_count, 0, 0); }

MessageQueueStore::MessageQueue::~MessageQueue() { sem_destroy(&queue_resource_count); }

MessageQueueStore::MessageQueue *MessageQueueStore::FetchQueue(MessageKey const key) {
    MessageQueue *queue;

    map_lock_.lock_shared();
    auto read_it = queues_.find(key);
    if (read_it != queues_.end()) {
        queue = read_it->second.get();
        map_lock_.unlock_shared();
    } else {
        map_lock_.unlock_shared();

        map_lock_.lock();
        auto write_it = queues_.insert(std::make_pair(key, std::make_shared<MessageQueue>())).first;
        queue = write_it->second.get();
        map_lock_.unlock();
    }

    assert(queue != nullptr);
    return queue;
}

void MessageQueueStore::Enqueue(MessageKey const key, RealTimeMessage const &message) {
    MessageQueue *message_queue = FetchQueue(key);

    message_queue->queue_lock.lock();
    message_queue->queue.push(message);
    message_queue->queue_lock.unlock();

    sem_post(&message_queue->queue_resource_count);
}

MessageQueueStore::MessageQueue *MessageQueueStore::BeginBlockingDequeue(MessageKey const key,
                                                                         int const wait_for_secs,
                                                                         RealTimeMessage *message) {
    MessageQueue *message_queue = FetchQueue(key);

    int rc;
    if (wait_for_secs > 0) {
        std::time_t expiry_timestamp;
        std::time(&expiry_timestamp);
        expiry_timestamp += wait_for_secs;

        timespec ts;
        ts.tv_sec = expiry_timestamp;
        ts.tv_nsec = 0;
        rc = sem_timedwait(&message_queue->queue_resource_count, &ts);
    } else {
        rc = sem_wait(&message_queue->queue_resource_count);
    }

    if (rc == -1 && errno == ETIMEDOUT) {
        return nullptr;
    }

    assert(rc == 0);

    message_queue->queue_lock.lock();
    *message = message_queue->queue.front();

    return message_queue;
}

void MessageQueueStore::EndBlockingDequeue(MessageQueue *message_queue, bool dequeue) {
    assert(message_queue != nullptr);

    if (dequeue) {
        message_queue->queue.pop();
    } else {
        sem_post(&message_queue->queue_resource_count);
    }
    message_queue->queue_lock.unlock();
}

void MessageQueueStore::Clear() {
    map_lock_.lock();
    queues_.clear();
    map_lock_.unlock();
}

MessageQueueStore *MessageQueueStoreInstance() { return &gMessageQueueStore; }

} // namespace e8
