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
 * @file        protocols.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       List of all protocols supported by security server.
 */

#include <protocols.h>
#include <cstddef>

namespace SecurityServer {

char const * const SERVICE_SOCKET_SHARED_MEMORY =
    "/tmp/.security-server-api-data-share.sock";
char const * const SERVICE_SOCKET_ECHO =
    "/tmp/security-server-api-echo.sock";
char const * const SERVICE_SOCKET_GET_GID =
    "/tmp/.security-server-api-get-gid.sock";
char const * const SERVICE_SOCKET_PRIVILEGE_BY_PID =
    "/tmp/.security-server-api-privilege-by-pid.sock";
char const * const SERVICE_SOCKET_APP_PERMISSIONS =
    "/tmp/.security-server-api-app-permissions.sock";
char const * const SERVICE_SOCKET_APP_PRIVILEGE_BY_NAME =
    "/tmp/.security-server-api-app-privilege-by-name.sock";
char const * const SERVICE_SOCKET_COOKIE_GET =
    "/tmp/.security-server-api-cookie-get.sock";
char const * const SERVICE_SOCKET_COOKIE_CHECK =
    "/tmp/.security-server-api-cookie-check.sock";
//TODO: Merge bellow socket with the one above. This should be done
//after security-server-api-cookie-check.sock will be protected by smack and has proper label
char const * const SERVICE_SOCKET_COOKIE_CHECK_TMP =
    "/tmp/.security-server-api-cookie-check-tmp.sock";
char const * const SERVICE_SOCKET_OPEN_FOR =
    "/tmp/.security-server-api-open-for.sock";
char const * const SERVICE_SOCKET_PASSWD_CHECK =
    "/tmp/.security-server-api-passwd-check.sock";
char const * const SERVICE_SOCKET_PASSWD_SET =
    "/tmp/.security-server-api-passwd-set.sock";

const size_t COOKIE_SIZE = 20;

const size_t MAX_PASSWORD_LEN = 32;
const unsigned int MAX_PASSWORD_HISTORY = 50;

const int SECURITY_SERVER_MAX_OBJ_NAME         = 30;

} // namespace SecurityServer
