/*
 *  Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Bumjin Im <bj.im@samsung.com>
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
 * @file        client-socket-privilege.cpp
 * @author      Zofia Abramowska (z.abramowska@samsung.com)
 * @version     1.0
 * @brief       This file constains implementation of socket privilege api.
 */
#include <memory>

#include <sys/socket.h>
#include <sys/smack.h>

#include <dpl/log/log.h>
#include <dpl/exception.h>

#include <socket-buffer.h>
#include <client-common.h>
#include <protocols.h>
#include <smack-check.h>

#include <security-server.h>
#include <security-server-common.h>

static int get_exec_path(pid_t pid, std::string &exe)
{
    using namespace SecurityServer;

    try{
        SocketBuffer send, recv;
        Serialization ser;
        ser.Serialize(send, pid);

        int result = sendToServer(
                SERVICE_SOCKET_EXEC_PATH,
                send.Pop(),
                recv);
        if(result != SECURITY_SERVER_API_SUCCESS)
            return result;

        Deserialization des;
        des.Deserialize(recv, result);
        if(result != SECURITY_SERVER_API_SUCCESS)
            return result;

        des.Deserialize(recv, exe);
        return result;
    } catch (SocketBuffer::Exception::Base &e) {
        LogDebug("SecurityServer::SocketBuffer::Exception " << e.DumpToString());
    } catch (std::exception &e) {
        LogDebug("STD exception " << e.what());
    } catch (...) {
        LogDebug("Unknown exception occured");
    }
    return SECURITY_SERVER_API_ERROR_UNKNOWN;
}

SECURITY_SERVER_API
int security_server_check_privilege_by_sockfd(int sockfd,
                                              const char *object,
                                              const char *access_rights)
{
    char *subject;
    int ret;
    std::string path;
    std::unique_ptr<char> subject_p;

    //for get socket options
    struct ucred cr;
    unsigned int len = sizeof(struct ucred);

    //SMACK runtime check
    if (!smack_runtime_check())
    {
        LogDebug("No SMACK support on device");
        return SECURITY_SERVER_API_SUCCESS;
    }

    ret = smack_new_label_from_socket(sockfd, &subject);
    if (ret == 0) {
        //after allocation we move ownership to smart pointer to let it do the cleanup
        subject_p.reset(subject);

    } else {
        ret = SECURITY_SERVER_API_ERROR_SERVER_ERROR;
        goto exit;
    }

    ret = getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &cr, &len);
    if (ret < 0) {
        LogError("Error in getsockopt(). Errno: " << strerror(errno));
        ret = SECURITY_SERVER_API_ERROR_ACCESS_DENIED;
        goto exit;
    }

    ret = security_server_check_privilege_by_pid(cr.pid, object, access_rights);

    SECURE_LOGD("security_server_check_privilege_by_pid returned %d", ret);

exit:
    //Getting path for logs
    if (SECURITY_SERVER_API_SUCCESS != get_exec_path(cr.pid, path))
        //If this is only for logs, do we want to log it as error?
        LogError("Failed to read executable path for process " << cr.pid);
    if (ret == SECURITY_SERVER_API_SUCCESS)
        SECURE_LOGD("SS_SMACK: caller_pid=%d, subject=%s, object=%s, access=%s, result=%d, caller_path=%s",
                cr.pid, subject_p.get() ? subject_p.get() : "NULL", object, access_rights, ret, path.c_str());
    else
        SECURE_LOGW("SS_SMACK: caller_pid=%d, subject=%s, object=%s, access=%s, result=%d, caller_path=%s",
                cr.pid, subject_p.get() ? subject_p.get() : "NULL", object, access_rights, ret, path.c_str());

    return ret;
}

SECURITY_SERVER_API
char *security_server_get_smacklabel_sockfd(int fd)
{
    char *label = NULL;

    if (!smack_check())
    {
        LogDebug("No SMACK support on device");
        label = (char*) malloc(1);
        if (label) label[0] = '\0';
        return label;
    }

    if (smack_new_label_from_socket(fd, &label) != 0)
    {
        LogError("Client ERROR: Unable to get socket SMACK label");
        return NULL;
    }

    return label;
}