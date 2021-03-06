/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2017 Haivision Systems Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <srtcore/srt.h>

int main( int argc, char** argv )
{
    int ss, st;
    struct sockaddr_in sa;
    int yes = 1;
    struct sockaddr_storage their_addr;

    printf("srt startup\n");
    srt_startup();

    printf("srt socket\n");
    ss = srt_socket(AF_INET, SOCK_DGRAM, 0);
    if ( ss == SRT_ERROR )
    {
        fprintf(stderr, "srt_socket: %s\n", srt_getlasterror_str());
        return 1;
    }

    printf("srt bind address\n");
    sa.sin_port = htons(atoi(argv[2]));
    if ( inet_pton(AF_INET, argv[1], &sa.sin_addr) != 1)
    {
        return 1;
    }

    printf("srt setsockflag\n");
    srt_setsockflag(ss, SRTO_TWOWAYDATA, &yes, sizeof yes);
    srt_setsockflag(ss, SRTO_RCVSYN, &yes, sizeof yes);

    printf("srt bind\n");
    st = srt_bind(ss, (struct sockaddr*)&sa, sizeof sa);
    if ( st == SRT_ERROR )
    {
        fprintf(stderr, "srt_bind: %s\n", srt_getlasterror_str());
        return 1;
    }

    printf("srt listen\n");
    st = srt_listen(ss, 1);
    if ( st == SRT_ERROR )
    {
        fprintf(stderr, "srt_listen: %s\n", srt_getlasterror_str());
        return 1;
    }

    printf("srt accept\n");
    int addr_size = sizeof their_addr;
    int their_fd = srt_accept(ss, (struct sockaddr *)&their_addr, &addr_size);

    printf("srt recvmsg\n");
    char msg[1024];
    st = srt_recvmsg(their_fd, msg, 1024);
    if (st == SRT_ERROR )
    {
        fprintf(stderr, "srt_recvmsg: %s\n", srt_getlasterror_str());
        return 1;
    }

    printf("Got msg of len %d: %s\n", st, msg);

    printf("srt close\n");
    st = srt_close(ss);
    if ( st == SRT_ERROR )
    {
        fprintf(stderr, "srt_close: %s\n", srt_getlasterror_str());
        return 1;
    }

    printf("srt cleanup\n");
    srt_cleanup();
    return 0;
}
