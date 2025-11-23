/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_SYNC_H
#define LMSHAO_LMCORE_SYNC_H

/**
 * @brief Synchronization primitives - Rust-style channels
 *
 * This module provides lock-free bounded channels for inter-thread communication,
 * similar to Rust's std::sync::mpsc and crossbeam-channel.
 *
 * Available channel types:
 * - SPSC: Single-Producer Single-Consumer (highest performance)
 * - MPSC: Multi-Producer Single-Consumer (like Rust std::sync::mpsc)
 * - SPMC: Single-Producer Multi-Consumer
 * - MPMC: Multi-Producer Multi-Consumer (like Rust crossbeam-channel)
 *
 * Example:
 * @code
 * using namespace lmshao::lmcore::sync;
 *
 * // SPSC - best for single producer/consumer
 * auto [tx, rx] = SpscChannel<int>(100);
 *
 * // MPSC - multiple producers, single consumer
 * auto [tx, rx] = MpscChannel<int>(100);
 * auto tx2 = tx; // Clone sender for another thread
 *
 * // SPMC - single producer, multiple consumers
 * auto [tx, rx] = SpmcChannel<int>(100);
 * auto rx2 = rx; // Clone receiver for another thread
 *
 * // MPMC - multiple producers and consumers
 * auto [tx, rx] = MpmcChannel<int>(100);
 * auto tx2 = tx;
 * auto rx2 = rx; // Clone both sender and receiver
 * @endcode
 */

#include "mpmc_channel.h"
#include "mpsc_channel.h"
#include "spmc_channel.h"
#include "spsc_channel.h"

#endif // LMSHAO_LMCORE_SYNC_H
