TEMPLATE = subdirs

SUBDIRS += \
    demo \
    test \
    mylib

demo.depends = mylib
test.depends = mylib
