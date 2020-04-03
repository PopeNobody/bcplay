#!/bin/bash

eval $(gpg < keys.asc)
return 0 2>/dev/null

echo "Don't run this file, source it!" >&2
exit 1
