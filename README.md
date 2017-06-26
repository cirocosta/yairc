# yairc

> A toy IRC server

Supported Commands:
- PING
- JOIN
- LIST
- NICK
- USER
- QUIT


## Using it

```sh
$ make 
$ ./yairc
```

## Development

Run the tests:

```sh
$ make clean && make test
```

### Conventions

I try to hold to the following conventions:

`typedef`s:

- `yi_NAME_t` names a struct
- `yi_NAME_e` names an enumeration
- `yi_NAME_u` names an union

methods:

- `yi_STRUCT..._new...`: object of `STRUCT` type creates and owns an object
- `yi_STRUCT..._delete...`: object of `STRUCT` type deletes an owned object

- `yi_..._create`:  creates a standalone object
- `yi_..._destroy`: destroys a standalone object

- `yi_..._add...`: adds an object to another object's internal collection (no ownership transfer)
- `yi_..._remove...`: removes from another object's internal collection

variable definitions, macro functions and constants
- `YI_ALL_UPPER_CASE`

## LICENSE

MPL 2.0. See `LICENSE`.

