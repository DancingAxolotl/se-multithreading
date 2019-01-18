TEMPLATE = subdirs

SUBDIRS += \
    src \
    3rd_party

src.depends = 3rd_party

