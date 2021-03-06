/*
 *  Copyright (c) 2000 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Rafal Krypa <r.krypa@samsung.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        base-service.h
 * @author      Lukasz Kostyra <l.kostyra@samsung.com>
 * @author      Rafal Krypa <r.krypa@samsung.com>
 * @brief       Implementation of security-manager base service
 */

#ifndef _SECURITY_MANAGER_BASE_SERVICE_
#define _SECURITY_MANAGER_BASE_SERVICE_

#include <service-thread.h>
#include <generic-socket-manager.h>
#include <message-buffer.h>
#include <connection-info.h>
#include <service_impl.h>

namespace SecurityManager {

class BaseServiceException
{
public:
    DECLARE_EXCEPTION_TYPE(SecurityManager::Exception, Base)
    DECLARE_EXCEPTION_TYPE(Base, InvalidAction)
};

class BaseService :
    public SecurityManager::GenericSocketService,
    public SecurityManager::ServiceThread<BaseService>
{
public:
    BaseService();
    virtual ServiceDescriptionVector GetServiceDescription() = 0;

    DECLARE_THREAD_EVENT(AcceptEvent, accept)
    DECLARE_THREAD_EVENT(WriteEvent, write)
    DECLARE_THREAD_EVENT(ReadEvent, process)
    DECLARE_THREAD_EVENT(CloseEvent, close)

    void accept(const AcceptEvent &event);
    void write(const WriteEvent &event);
    void process(const ReadEvent &event);
    void close(const CloseEvent &event);

protected:
    ServiceImpl serviceImpl;

    ConnectionInfoMap m_connectionInfoMap;

    /**
     * Retrieves ID (UID and PID) of peer connected to socket
     *
     * @param[in]  sock Socket file descriptor
     * @param[out] uid PID of connected peer.
     * @param[out] pid PID of connected peer.
     * @param[out] smackLabel Smack label of connected peer.
     *
     * @return True if peer ID was successfully retrieved, false otherwise.
     */
    bool getPeerID(int sock, uid_t &uid, pid_t &pid, std::string &smackLabel);

    /**
     * Handle request from a client
     *
     * @param  conn        Socket connection information
     * @param  buffer      Raw received data buffer
     * @param  interfaceID identifier used to distinguish source socket
     * @return             true on success
     */
    virtual bool processOne(const ConnectionID &conn,
                            MessageBuffer &buffer,
                            InterfaceID interfaceID) = 0;
};

} // namespace SecurityManager

#endif // _SECURITY_MANAGER_BASE_SERVICE_
