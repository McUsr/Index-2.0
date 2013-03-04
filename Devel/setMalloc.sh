#!/bin/bash
# This script is for sourcing, so that we can make malloc behave like we want.
export MallocStackLogging=1
# records all stacks so tools like leaks can be used.
export MallocErrorAbort=1
