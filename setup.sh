#!/bin/bash

eval $(gpg < keys.asc)

exec bash
