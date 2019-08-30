// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.

// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <chrono>
#include "InetAddress.h"

namespace smooth::core::network
{
    class SocketDispatcher;

    class ISocketBackOff;

    /// Interface for sockets
    class ISocket
    {
        friend class smooth::core::network::SocketDispatcher;
        public:
            static const int INVALID_SOCKET = -1;
#ifdef ESP_PLATFORM

            // lwip doesn't signal SIGPIPE
            static const int SEND_FLAGS = 0;
#else

            // Disable SIGPIPE during send()-calls.
            static const int SEND_FLAGS = MSG_NOSIGNAL;
#endif

            virtual ~ISocket() = default;

            /// Initiates the connection to the provided IP. After this call events will arrive
            /// via the response methods for data available, TX buffer empty, connection status etc.
            /// \param ip The address to connect to (an instance of either IPv4 or IPv6).
            /// \return true if the socket could be started and connection attempt initiated (but possibly not succeeded
            // or yet completed)
            virtual bool start(std::shared_ptr<InetAddress> ip) = 0;

            /// Stops the socket, disconnected if currently connected.
            virtual void stop(const char* reason) = 0;

            /// Restarts the socket, disconnecting if currently connected.
            /// \return true if the internal call to start() succeeds.
            virtual bool restart() = 0;

            /// Returns a value static if the socket is active (true) or not (false). Says nothing about if the
            /// socket is connected or not (use the response method for ConnectionStatusEvent for that).
            /// \return true or false, depending on status.
            [[nodiscard]] virtual bool is_active() const = 0;

            /// Returns true if the last send attempt has expired
            /// \return true if the socket has not been able to send the data within the set limit.
            [[nodiscard]] virtual bool has_send_expired() const = 0;

            /// Returns true if the last send attempt has expired
            /// \return true if the socket has not been able to send the data within the set limit.
            [[nodiscard]] virtual bool has_receive_expired() const = 0;

            /// Returns the socket id.
            /// \return The socket id, possibly INVALID_SOCKET.
            [[nodiscard]] virtual int get_socket_id() const = 0;

            /// Return true if the socket is a server-socket
            /// \return true if the socket is a server socket, otherwise false.
            [[nodiscard]] virtual bool is_server() const = 0;

            virtual void set_send_timeout(std::chrono::milliseconds timeout) = 0;

            virtual void set_receive_timeout(std::chrono::milliseconds timeout) = 0;

            [[nodiscard]] virtual std::chrono::milliseconds get_receive_timeout() const = 0;

            [[nodiscard]] virtual std::chrono::milliseconds get_send_timeout() const = 0;

        protected:
            [[nodiscard]] virtual bool is_connected() const = 0;

            virtual void readable(ISocketBackOff& ops) = 0;

            virtual void writable() = 0;

            [[nodiscard]] virtual bool has_data_to_transmit() = 0;

            [[nodiscard]] virtual bool internal_start() = 0;

            virtual void publish_connected_status() = 0;

            virtual void stop_internal() = 0;

            virtual void clear_socket_id() = 0;
    };
}
