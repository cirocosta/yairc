---
title: 'IRC protocol summarized'
author: Ciro S. Costa
date: Aug 21, 2015
---

## General Characteristics

- servers are uniquely identified by their name (max length of 63 chars)

- for each client all servers must have: a netwide UID and the server which introduced the client.

- users are distinguished by a unique nickname (max of 9 chars)

- services are are distinguished from other services by a unique nickname and a server name.

- channel names starts with &|#|+|! of length up to 50 chats w/out whitespaces or control G (^G) or a comma, being case insensitive.

- the protocol is an 8-bit protocol (octets).


## Messages

Three parts: optional **prefix** (indicated when omitted), **command** and **parameters** (max of 15)

Only servers use prefix (a client is only allowed to send the registered nickname as a prefix).

**command** is a valid irc command or a 3 digit number in ASCII.

```abnf
SPACE       = %x20        
crlf        = %x0D %x0A   
nospcrlfcl  = %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
              ; any octet except NUL, CR, LF, " " and ":" ([^{NUL}{CR}{LF} :])
middle      = nospcrlfcl *( ":" / nospcrlfcl )
trailing    = *( ":" / " " / nospcrlfcl )


message     =   [ ":" prefix SPACE ] command [ params ] crlf
prefix      =   servername / (nickname [ [ "!" user ] "@" host ] )
command     =   1*letter / 3digit
params      =   *14( SPACE middle ) [ SPACE ":" trailing ]
            =/  14( SPACE middle ) [ SPACE [ ":" ] trailing ]
```

