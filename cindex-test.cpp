#include <clang-c/Index.h>
#include <iostream>
#include <type_traits>

template <typename E> constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

CXChildVisitResult attr_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    std::cout << "attr_visitor: " << clang_getCString(clang_getCursorDisplayName(cursor))
              << std::endl;
    CXCursorKind kind = clang_getCursorKind(cursor);
    std::cout << "- attr_visitor kind: " << clang_getCString(clang_getCursorKindSpelling(kind))
              << " aka " << to_integral(kind) << std::endl;
    if (kind == CXCursor_ObjCBridgeAttr || kind == CXCursor_ObjCBridgeMutableAttr || kind == CXCursor_ObjCBridgeRelatedAttr) {
        *(bool *)client_data = true;
        return CXChildVisit_Break;
    }
    CXType type = clang_getCursorType(cursor);
    std::cout << "- attr_visitor type: " << clang_getCString(clang_getTypeSpelling(type)) << " aka "
              << to_integral(type.kind) << std::endl;
    CXType attributedType = clang_Type_getModifiedType(type);
    std::cout << "- attr_visitor attributedType: "
              << clang_getCString(clang_getTypeSpelling(attributedType)) << " aka "
              << to_integral(attributedType.kind) << std::endl;
    return CXChildVisit_Recurse;
}

bool is_objc_object_or_class(CXType type, CXCursor cursor) {
    if (type.kind == CXType_ObjCObject || type.kind == CXType_ObjCId ||
        type.kind == CXType_ObjCInterface || type.kind == CXType_ObjCClass) {
        return true;
    }
    // Check for attributed types too
    if (type.kind == CXType_Attributed) {
        CXType attributedType = clang_Type_getModifiedType(type);
        std::cout << "- attributed: " << clang_getCString(clang_getTypeSpelling(attributedType))
                  << " aka " << to_integral(attributedType.kind) << std::endl;
        clang_visitChildren(cursor, attr_visitor, nullptr);
        return is_objc_object_or_class(attributedType, cursor);
    }
    // Check for attributed cursors too
    if (clang_Cursor_hasAttrs(cursor)) {
        CXCursorKind kind = clang_getCursorKind(cursor);
        std::cout << "attributed cursor: " << clang_getCString(clang_getCursorDisplayName(cursor))
                  << " kind: " << clang_getCString(clang_getCursorKindSpelling(kind))
                  << " num: " << clang_Cursor_hasAttrs(cursor) << std::endl;
        bool objc_attributed = false;
        clang_visitChildren(cursor, attr_visitor, &objc_attributed);
        if (objc_attributed) {
            return true;
        }
    }
    // Check for pointer types too
    if (type.kind == CXType_Pointer) {
        CXType pointeeType = clang_getPointeeType(type);
        std::cout << "- pointee: " << clang_getCString(clang_getTypeSpelling(pointeeType))
                  << " aka " << to_integral(pointeeType.kind) << std::endl;
        return is_objc_object_or_class(pointeeType, cursor);
    }
    // Check for elaborated types too
    if (type.kind == CXType_Elaborated) {
        CXType namedType = clang_Type_getNamedType(type);
        std::cout << "- elaborated: " << clang_getCString(clang_getTypeSpelling(namedType))
                  << " aka " << to_integral(namedType.kind) << std::endl;
        return is_objc_object_or_class(namedType, cursor);
    }
    // Check for canonical types too
    if (type.kind == CXType_Typedef) {
        CXType canonicalType = clang_getCanonicalType(type);
        std::cout << "- canonical: " << clang_getCString(clang_getTypeSpelling(canonicalType))
                  << " aka " << to_integral(canonicalType.kind) << std::endl;
        return is_objc_object_or_class(canonicalType, cursor);
    }
    // Check for record types too
    if (type.kind == CXType_Record) {
        CXCursor declCursor = clang_getTypeDeclaration(type);
        std::cout << "- record decl cursor: "
                  << clang_getCString(clang_getCursorDisplayName(declCursor)) << std::endl;
        // CXCursorKind declKind = clang_getCursorKind(declCursor);
        CXType declType = clang_getCursorType(declCursor);
        std::cout << "- record decl type: " << clang_getCString(clang_getTypeSpelling(declType))
                  << " aka " << to_integral(declType.kind) << std::endl;
        CXType recordType = clang_getCanonicalType(type);
        std::cout << "- record: " << clang_getCString(clang_getTypeSpelling(recordType)) << " aka "
                  << to_integral(recordType.kind) << std::endl;
        return is_objc_object_or_class(recordType, declCursor);
    }
    return false;
}

bool is_objc_selector(CXType type) {
    if (type.kind == CXType_ObjCSel) {
        return true;
    }
    // Check for pointer types too
    if (type.kind == CXType_Pointer) {
        CXType pointeeType = clang_getPointeeType(type);
        return is_objc_selector(pointeeType);
    }
    return false;
}

CXChildVisitResult function_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    CXCursorKind kind = clang_getCursorKind(cursor);

    if (kind == CXCursor_FunctionDecl || kind == CXCursor_ObjCInstanceMethodDecl ||
        kind == CXCursor_ObjCClassMethodDecl) {
        unsigned num_args = clang_Cursor_getNumArguments(cursor);
        std::cout << "Function/Method: " << clang_getCString(clang_getCursorDisplayName(cursor))
                  << std::endl;

        for (unsigned i = 0; i < num_args; ++i) {
            CXType argType = clang_getArgType(clang_getCursorType(cursor), i);
            std::cout << "- Parameter " << i << " is "
                      << clang_getCString(clang_getTypeSpelling(argType)) << " aka "
                      << to_integral(argType.kind) << std::endl;

            if (is_objc_object_or_class(argType, cursor)) {
                std::cout << "- Parameter " << i
                          << " is an ObjC object or class (or a pointer to one)." << std::endl;
            }

            if (is_objc_selector(argType)) {
                std::cout << "- Parameter " << i << " is an ObjC selector (or a pointer to one)."
                          << std::endl;
            }
        }
    }

    return CXChildVisit_Recurse;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <header-file>" << std::endl;
        return 1;
    }

    const char *args[] = {"-Xclang", "-triple",    "-Xclang", "arm64-apple-ios17.0.0",
                          "-x",      "objective-c"};

    CXIndex index        = clang_createIndex(0, 0);
    CXTranslationUnit tu = clang_parseTranslationUnit(
        index, argv[1], args, sizeof(args) / sizeof(args[0]), nullptr, 0, CXTranslationUnit_None);
    if (tu == nullptr) {
        std::cerr << "Failed to parse translation unit." << std::endl;
        return 1;
    }

    CXCursor rootCursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(rootCursor, function_visitor, nullptr);

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);

    return 0;
}