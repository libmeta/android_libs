#pragma once

#define IMPORT_CONSTRUCTOR_DELETE(CLASS_NAME)          \
    CLASS_NAME(const CLASS_NAME&) = delete;            \
    CLASS_NAME(CLASS_NAME&&) = delete;                 \
    CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
    CLASS_NAME& operator=(CLASS_NAME&&) = delete;

#define IMPORT_CONSTRUCTOR_DEFAULT(CLASS_NAME) \
    explicit CLASS_NAME() = default;
