#!/bin/bash

cpplint() {
    python2 ./tools/cpplint.py "$@"
}

cpplint --extensions=hpp,cpp \
	--filter=-runtime/references,-build/include_order,-whitespace/line_length,-readability/streams,-runtime/indentation_namespace,-runtime/int,-readability/namespace,-runtime/explicit,-build/namespaces "$@"
