/*
 * Copyright (c) 2016 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "publisher.h"

#include "protocol.h"

namespace particle {

namespace protocol {

void Publisher::add_ack_handler(message_id_t msg_id, CompletionHandler handler) {
    protocol->add_ack_handler(msg_id, std::move(handler), SEND_EVENT_ACK_TIMEOUT);
}

ProtocolError Publisher::send_event(MessageChannel& channel, const char* event_name,
            const char* data, int ttl, EventType::Enum event_type, int flags,
            system_tick_t time, CompletionHandler handler) {
    bool is_system_event = is_system(event_name);
    bool rate_limited = is_rate_limited(is_system_event, time);
    if (rate_limited) {
        g_rateLimitedEventsCounter++;
        return BANDWIDTH_EXCEEDED;
    }

    Message message;
    channel.create(message);
    bool confirmable = channel.is_unreliable();
    if (flags & EventType::NO_ACK) {
        confirmable = false;
    } else if (flags & EventType::WITH_ACK) {
        confirmable = true;
    }

    // Adjust the maximum event payload size if MAX_TRANSMIT_MESSAGE_SIZE property
	// indicates that we cannot transmit over a certain limit.
    const auto max_transmit_message_size = protocol->get_max_transmit_message_size();
    size_t data_size = 0;
    if (data) {
        size_t max_size = MAX_EVENT_DATA_LENGTH;
        if (max_transmit_message_size && max_transmit_message_size < MAX_EVENT_MESSAGE_SIZE) {
            const auto overhead = MAX_EVENT_MESSAGE_SIZE - MAX_EVENT_NAME_LENGTH - MAX_EVENT_DATA_LENGTH;
            const size_t event_name_length = strnlen(event_name, MAX_EVENT_NAME_LENGTH);
            max_size = std::min(max_size, max_transmit_message_size - overhead - event_name_length);
        }
        data_size = strnlen(data, max_size);
    }
    size_t msglen = Messages::event(message.buf(), 0, event_name, data, data_size, ttl,
            event_type, confirmable);
    message.set_length(msglen);
    const ProtocolError result = channel.send(message);
    if (result == NO_ERROR) {
        // Register completion handler only if acknowledgement was requested explicitly
        if ((flags & EventType::WITH_ACK) && message.has_id()) {
            add_ack_handler(message.get_id(), std::move(handler));
        } else {
            handler.setResult();
        }
    }
    return result;
}

} // protocol

} // particle
