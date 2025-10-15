#include <moe_alloc.h>
#include <moe_string.h>

#include <stdio.h>
#include <string.h>

#include "moe_list.h"
#include "stb_include.h"

#include <quickjs.h>

const char MOE_PROC_MACRO_DERIVES_MACRO_NAME[] = "MOE_PROC_MACRO_DERIVES";
const int MOE_PROC_MACRO_MAX_ARG_NUM = 8;
const int MOE_PROC_MACRO_MAX_DERIVE_ITEM_NUM = 8;

typedef struct moe_derive_arg_s {
    moe_string_t* arg_type;
    moe_string_t* arg_name;
} moe_derive_arg_t;

moe_derive_arg_t moe_derive_arg_create(const char* arg_type, int type_capacity, const char* arg_name, int name_capacity) {
    moe_derive_arg_t arg;
    arg.arg_type = moe_string_alloc(arg_type, type_capacity);
    arg.arg_name = moe_string_alloc(arg_name, name_capacity);
    return arg;
}

void moe_derive_arg_destroy(moe_derive_arg_t* arg) {
    if (arg == NULL) {
        return;
    }

    moe_string_free(arg->arg_type);
    moe_string_free(arg->arg_name);
}

typedef struct moe_derive_item_s {
    moe_string_t* derive_handler_name;
    int arg_num;
    moe_derive_arg_t* args;
} moe_derive_item_t;

void moe_derive_item_create(moe_derive_item_t* item, const char* derive_handler_name, int arg_num) {
    if (item == NULL) {
        return;
    }

    item->derive_handler_name = moe_string_alloc(derive_handler_name, 0);
    item->arg_num = arg_num;
    item->args = MOE_ALLOC_RANGE(moe_derive_arg_t, arg_num);
}

void moe_derive_item_destroy(moe_derive_item_t* item) {
    if (item == NULL) {
        return;
    }

    moe_string_free(item->derive_handler_name);
    for (int i = 0; i < item->arg_num; i++) {
        moe_derive_arg_destroy(&item->args[i]);
    }
    MOE_FREE(item->args);
}

#define MOE_TOKEN_XXX                                            \
    X(MOE_TOKEN_UNKNOWN, "moe_token_type_unknown")               \
    X(MOE_TOKEN_IDENTIFIER, "moe_token_type_identifier")         \
    X(MOE_TOKEN_PUNCTUATION, "moe_token_type_punctuation")       \
    X(MOE_TOKEN_STRING_LITERAL, "moe_token_type_string_literal") \
    X(MOE_TOKEN_INT_LITERAL, "moe_token_type_int_literal")       \
    X(MOE_TOKEN_FLOAT_LITERAL, "moe_token_type_float_literal")

typedef enum moe_token_type_e {
#define X(name, string) name,
    MOE_TOKEN_XXX
#undef X
} moe_token_type_t;

const char* moe_token_type_to_string(moe_token_type_t type) {
#define X(name, string) string,
    static const char* type_strings[] = {MOE_TOKEN_XXX};
#undef X
    return type_strings[type];
}

typedef struct moe_token_s {
    union {
        int int_value;
        float float_value;
        moe_string_t* str_value;
    };
    moe_token_type_t type;
} moe_token_t;

moe_token_t* moe_token_create_identifier(const char* identifier, int len) {
    moe_token_t* token = MOE_ALLOC(moe_token_t);
    token->type = MOE_TOKEN_IDENTIFIER;
    token->str_value = moe_string_alloc(identifier, len);
    return token;
}

moe_token_t* moe_token_create_string_literal(const char* str, int len) {
    moe_token_t* token = MOE_ALLOC(moe_token_t);
    token->type = MOE_TOKEN_STRING_LITERAL;
    token->str_value = moe_string_alloc(str, len);
    return token;
}

moe_token_t* moe_token_create_punctuation(char punctuation) {
    moe_token_t* token = MOE_ALLOC(moe_token_t);
    token->type = MOE_TOKEN_PUNCTUATION;
    token->int_value = (int) punctuation;
    return token;
}

moe_token_t* moe_token_create_int_literal(int value) {
    moe_token_t* token = MOE_ALLOC(moe_token_t);
    token->type = MOE_TOKEN_INT_LITERAL;
    token->int_value = value;
    return token;
}

moe_token_t* moe_token_create_float_literal(float value) {
    moe_token_t* token = MOE_ALLOC(moe_token_t);
    token->type = MOE_TOKEN_FLOAT_LITERAL;
    token->float_value = value;
    return token;
}

void moe_token_print(const moe_token_t* token) {
    if (token == MOE_NULL) {
        printf("NULL token\n");
        return;
    }

    const char* type_str = moe_token_type_to_string(token->type);
    switch (token->type) {
        case MOE_TOKEN_IDENTIFIER:
        case MOE_TOKEN_STRING_LITERAL:
            printf("Token(type=%s, value=\"%s\")\n", type_str, moe_string_get(token->str_value));
            break;
        case MOE_TOKEN_PUNCTUATION:
            printf("Token(type=%s, value='%c')\n", type_str, (char) token->int_value);
            break;
        case MOE_TOKEN_INT_LITERAL:
            printf("Token(type=%s, value=%d)\n", type_str, token->int_value);
            break;
        case MOE_TOKEN_FLOAT_LITERAL:
            printf("Token(type=%s, value=%f)\n", type_str, token->float_value);
            break;
        default:
            MOE_UNREACHABLE();
    }
}

void moe_token_free(moe_token_t* token) {
    if (token == NULL) {
        return;
    }

    if (token->type == MOE_TOKEN_IDENTIFIER ||
        token->type == MOE_TOKEN_STRING_LITERAL) {
        moe_string_free(token->str_value);
    }

    MOE_FREE(token);
}

typedef struct moe__struct_metadata_token_stream_node_s {
    moe_list_node_t list_node;
    moe_token_t* token;
} moe__struct_metadata_token_stream_node_t;

moe__struct_metadata_token_stream_node_t* moe_struct_metadata_token_stream_node_alloc(moe_token_t* token) {
    moe__struct_metadata_token_stream_node_t* node =
            MOE_ALLOC(moe__struct_metadata_token_stream_node_t);
    node->token = token;
    moe_list_node_init(&node->list_node);
    return node;
}

void moe_struct_metadata_token_stream_node_free(moe__struct_metadata_token_stream_node_t* node) {
    if (node == NULL) {
        return;
    }

    moe_token_free(node->token);
    MOE_FREE(node);
}

typedef struct moe_derive_struct_metadata_s {
    moe_list_node_t token_stream_node;
} moe_struct_metadata_t;

void moe_struct_metadata_init(moe_struct_metadata_t* metadata) {
    moe_list_node_init(&metadata->token_stream_node);
}

void moe_derive_struct_metadata_add_token(
        moe_struct_metadata_t* metadata,
        moe_token_t* token) {
    if (metadata == NULL) {
        return;
    }

    moe__struct_metadata_token_stream_node_t* node =
            moe_struct_metadata_token_stream_node_alloc(token);
    moe_list_add_tail(&metadata->token_stream_node, &node->list_node);
}

void moe_derive_struct_metadata_clear_iterator(moe_list_node_t* node, void* ctx) {
    moe__struct_metadata_token_stream_node_t* token_node =
            moe_list_entry(node, moe__struct_metadata_token_stream_node_t, list_node);
    moe_struct_metadata_token_stream_node_free(token_node);
}

void moe_derive_struct_metadata_print(moe_struct_metadata_t* metadata) {
    if (metadata == NULL) {
        return;
    }

    moe_list_iterator_t it;
    moe_list_iterator_init(&it, &metadata->token_stream_node);
    while (moe_list_iterator_next(&it)) {
        moe__struct_metadata_token_stream_node_t* token_node =
                moe_list_entry(moe_list_iterator_get(&it), moe__struct_metadata_token_stream_node_t, list_node);
        moe_token_print(token_node->token);
    }
}

void moe_struct_metadata_destroy(moe_struct_metadata_t* metadata) {
    if (metadata == NULL) {
        return;
    }

    moe_list_iterate_safe(
            &metadata->token_stream_node,
            moe_derive_struct_metadata_clear_iterator,
            NULL);
}

typedef struct moe_derived_struct_s {
    moe_string_t* struct_name;
    int derive_item_num;
    moe_derive_item_t* derive_items;

    moe_struct_metadata_t metadata;
} moe_derived_struct_t;

moe_derived_struct_t* moe_derived_struct_alloc(const char* struct_name, int derive_item_num) {
    moe_derived_struct_t* derived_struct = MOE_ALLOC(moe_derived_struct_t);
    derived_struct->struct_name = moe_string_alloc(struct_name, 0);
    derived_struct->derive_item_num = derive_item_num;
    derived_struct->derive_items = MOE_ALLOC_RANGE(moe_derive_item_t, derive_item_num);

    moe_struct_metadata_init(&derived_struct->metadata);
    return derived_struct;
}

void moe_derived_struct_free(moe_derived_struct_t* derived_struct) {
    if (derived_struct == NULL) {
        return;
    }

    moe_string_free(derived_struct->struct_name);
    for (int i = 0; i < derived_struct->derive_item_num; i++) {
        moe_derive_item_destroy(&derived_struct->derive_items[i]);
    }
    MOE_FREE(derived_struct->derive_items);
    moe_struct_metadata_destroy(&derived_struct->metadata);

    MOE_FREE(derived_struct);
}

#define moe_next_token(lex) \
    MOE_ASSERT(stb_c_lexer_get_token(lex))

#define moe_expect_token(lex, expected_token) \
    MOE_ASSERT((lex)->token == (expected_token));

#define moe_expect_next_token(lex, expected_token) \
    moe_next_token(lex);                           \
    moe_expect_token(lex, expected_token);

void parse_args(stb_lexer* lex) {
    moe_expect_next_token(lex, CLEX_id);
    moe_expect_next_token(lex, '(');
    moe_expect_next_token(lex, CLEX_id);
    moe_expect_next_token(lex, ',');
    moe_expect_next_token(lex, CLEX_id);
    moe_expect_next_token(lex, ')');
}

void parse_derive_item(stb_lexer* lex, moe_derive_item_t* item) {
    moe_expect_next_token(lex, CLEX_id);
    moe_derive_item_create(item, lex->string, 0);

    // todo: handle args
    while ((moe_next_token(lex), lex->token) == ',') {
        parse_args(lex);
    }
}

int try_parse_derive(stb_lexer* lex, const moe_string_t* struct_name, moe_derive_item_t* item) {
    if (lex->token != CLEX_id ||
        strcmp(lex->string, MOE_PROC_MACRO_DERIVES_MACRO_NAME) != 0) {
        return 0;
    }

    moe_expect_next_token(lex, '(');
    moe_expect_next_token(lex, CLEX_id);

    if (strcmp(moe_string_get_const(struct_name), lex->string) != 0) {
        printf("Warning: Struct name mismatch: expected %s, got %s\n",
               moe_string_get_const(struct_name),
               lex->string);
        return -1;
    }

    moe_expect_next_token(lex, ',');
    parse_derive_item(lex, item);

    moe_next_token(lex);
    if (lex->token == ';') {
        moe_next_token(lex);
    }
    return 1;
}

int try_parse_derived_struct_body(
        stb_lexer* lex,
        const moe_string_t* struct_name,
        moe_derived_struct_t* derived_struct) {
    moe_bool is_typedef = MOE_FALSE;
    if (strcmp(lex->string, "typedef") == 0) {
        moe_next_token(lex);// typedef struct struct_name {} type_name;
        is_typedef = MOE_TRUE;
    }

    if (lex->token != CLEX_id ||
        strcmp(lex->string, "struct") != 0) {
        return 0;
    }

    moe_expect_next_token(lex, CLEX_id);
    if (strcmp(moe_string_get_const(struct_name), lex->string) != 0) {
        printf("Warning: Struct name mismatch: expected %s, got %s\n",
               moe_string_get_const(struct_name),
               lex->string);
        return -1;
    }

    moe_expect_next_token(lex, '{');
    while (1) {
        moe_next_token(lex);
        if (lex->token == '}') {
            break;
        }

        moe_token_t* token = MOE_NULL;
        if (lex->token == CLEX_id) {
            token = moe_token_create_identifier(lex->string, lex->string_len);
        } else if (lex->token == CLEX_sqstring || lex->token == CLEX_dqstring) {
            token = moe_token_create_string_literal(lex->string, lex->string_len);
        } else if (lex->token == CLEX_intlit) {
            token = moe_token_create_int_literal(lex->int_number);
        } else if (lex->token == CLEX_floatlit) {
            token = moe_token_create_float_literal(lex->real_number);
        } else {
            token = moe_token_create_punctuation((char) lex->token);
        }

        moe_derive_struct_metadata_add_token(
                &derived_struct->metadata, token);
    }

    if (is_typedef) {
        moe_expect_next_token(lex, CLEX_id);// type_name
    }
    moe_expect_next_token(lex, ';');

    return 1;
}

moe_derived_struct_t* parse_derived_struct(stb_lexer* lex) {
    if (lex->token != CLEX_id) {
        return NULL;
    }

    if (strcmp(lex->string, MOE_PROC_MACRO_DERIVES_MACRO_NAME) != 0) {
        return NULL;
    }

    moe_expect_next_token(lex, '(');
    moe_expect_next_token(lex, CLEX_id);

    moe_derived_struct_t* derived_struct =
            moe_derived_struct_alloc(
                    lex->string,
                    MOE_PROC_MACRO_MAX_DERIVE_ITEM_NUM);

    const moe_string_t* struct_name = derived_struct->struct_name;
    int derive_count = 0;

    moe_expect_next_token(lex, ',');

    parse_derive_item(lex, &derived_struct->derive_items[derive_count++]);

    moe_next_token(lex);
    if (lex->token == ';') {
        moe_next_token(lex);
    }

    while (derive_count < MOE_PROC_MACRO_MAX_DERIVE_ITEM_NUM) {
        int ret = try_parse_derive(lex, struct_name, &derived_struct->derive_items[derive_count]);
        if (ret == 1) {
            derive_count++;
        } else if (ret == 0) {
            break;
        }
    }

    derived_struct->derive_item_num = derive_count;
    {
        int result = try_parse_derived_struct_body(lex, struct_name, derived_struct);
        if (result == 0) {
            printf("Error: Failed to parse struct body for struct %s\n", moe_string_get_const(struct_name));
            moe_derived_struct_free(derived_struct);
            return NULL;
        } else if (result == -1) {
            printf("Error: Struct name mismatch when parsing struct body for struct %s\n", moe_string_get_const(struct_name));
            moe_derived_struct_free(derived_struct);
            return NULL;
        }
    }

    return derived_struct;
}

typedef struct moe_js_runtime_s {
    JSRuntime* rt;
    JSContext* ctx;

    JSValue global_obj;
} moe_js_runtime_t;

static JSValue next_token_func_impl(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv) {
    return JS_UNDEFINED;
}

moe_js_runtime_t* moe_js_runtime_alloc() {
    moe_js_runtime_t* js_runtime = MOE_ALLOC(moe_js_runtime_t);
    js_runtime->rt = JS_NewRuntime();
    js_runtime->ctx = JS_NewContext(js_runtime->rt);

    js_runtime->global_obj = JS_GetGlobalObject(js_runtime->ctx);

    return js_runtime;
}

void moe_js_runtime_free(moe_js_runtime_t* js_runtime) {
    if (js_runtime == NULL) {
        return;
    }

    JS_FreeValue(js_runtime->ctx, js_runtime->global_obj);

    JS_FreeContext(js_runtime->ctx);
    JS_FreeRuntime(js_runtime->rt);
    MOE_FREE(js_runtime);
}

JSValue from_token_to_js_token_object(JSContext* ctx, const moe_token_t* token) {
    JSValue js_token = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, js_token, "type", JS_NewString(ctx, moe_token_type_to_string(token->type)));
    switch (token->type) {
        case MOE_TOKEN_IDENTIFIER:
        case MOE_TOKEN_STRING_LITERAL: {
            JS_SetPropertyStr(ctx, js_token, "value", JS_NewString(ctx, moe_string_get_const(token->str_value)));
            break;
        }
        case MOE_TOKEN_PUNCTUATION: {
            char punct_str[2] = {(char) token->int_value, '\0'};
            JS_SetPropertyStr(ctx, js_token, "value", JS_NewString(ctx, punct_str));
            break;
        }
        case MOE_TOKEN_INT_LITERAL: {
            JS_SetPropertyStr(ctx, js_token, "value", JS_NewInt32(ctx, token->int_value));
            break;
        }
        case MOE_TOKEN_FLOAT_LITERAL: {
            JS_SetPropertyStr(ctx, js_token, "value", JS_NewFloat64(ctx, token->float_value));
            break;
        }
        default:
            MOE_UNREACHABLE();
    }
    return js_token;
}

JSValue from_token_stream_to_js_array(JSContext* ctx, moe_list_node_t* token_stream_head) {
    JSValue arr = JS_NewArray(ctx);
    moe_list_iterator_t it;
    moe_list_iterator_init(&it, token_stream_head);
    int index = 0;
    while (moe_list_iterator_next(&it)) {
        moe__struct_metadata_token_stream_node_t* token_node =
                moe_list_entry(moe_list_iterator_get(&it), moe__struct_metadata_token_stream_node_t, list_node);
        moe_token_t* token = token_node->token;

        JSValue js_token = from_token_to_js_token_object(ctx, token);
        JS_SetPropertyUint32(ctx, arr, index++, js_token);
    }
    return arr;
}

moe_string_t* read_file(const char* filename) {
    FILE* f = fopen(filename, "rb");// in binary mode
    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    moe_string_t* str = moe_string_alloc(NULL, size);
    fread(moe_string_get(str), 1, size, f);

    fclose(f);
    return str;
}

moe_string_t* generate_code_for_derived_struct(
        moe_js_runtime_t* js_runtime,
        JSValue js_token_array,
        const moe_derived_struct_t* derived_struct) {
    moe_string_t* output = MOE_NULL;

    for (int i = 0; i < derived_struct->derive_item_num; i++) {
        moe_derive_item_t* item = &derived_struct->derive_items[i];
        printf("  Derive item: %s\n", moe_string_get(item->derive_handler_name));
        for (int j = 0; j < item->arg_num; j++) {
            moe_derive_arg_t* arg = &item->args[j];
            printf("    Arg: %s %s\n", moe_string_get(arg->arg_type), moe_string_get(arg->arg_name));
        }

        JSValue callfunc = JS_GetPropertyStr(
                js_runtime->ctx, js_runtime->global_obj,
                moe_string_get_const(item->derive_handler_name));

        if (JS_IsFunction(js_runtime->ctx, callfunc)) {
            JSValue argv[2];
            argv[0] = JS_NewString(js_runtime->ctx, moe_string_get_const(derived_struct->struct_name));
            argv[1] = js_token_array;
            JSValue ret = JS_Call(
                    js_runtime->ctx, callfunc, js_runtime->global_obj,
                    2, argv);
            JS_FreeValue(js_runtime->ctx, argv[0]);

            if (JS_IsException(ret)) {
                JSValue exception = JS_GetException(js_runtime->ctx);
                const char* error_message = JS_ToCString(js_runtime->ctx, exception);
                printf("Error: Exception when calling derive handler %s: %s\n",
                       moe_string_get(item->derive_handler_name),
                       error_message);
                JS_FreeCString(js_runtime->ctx, error_message);
                JS_FreeValue(js_runtime->ctx, exception);
            } else {
                const char* generated_code = JS_ToCString(js_runtime->ctx, ret);
                printf("Generated code from derive handler %s:\n%s\n",
                       moe_string_get(item->derive_handler_name),
                       generated_code);

                output = moe_string_alloc(generated_code, 0);

                JS_FreeCString(js_runtime->ctx, generated_code);
                JS_FreeValue(js_runtime->ctx, ret);
            }

            JS_FreeValue(js_runtime->ctx, callfunc);
        } else {
            printf("Warning: Derive handler %s is not a function\n", moe_string_get(item->derive_handler_name));
            JS_FreeValue(js_runtime->ctx, callfunc);
            continue;
        }
    }

    return output;
}

int load_proc_macro_scripts(moe_js_runtime_t* js_runtime, const char* script_path) {
    moe_string_t* script = read_file(script_path);
    if (script == NULL) {
        printf("Error: Failed to read proc macro script file: %s\n", script_path);
        return 1;
    }

    printf("%s\n", moe_string_get(script));

    JSValue ret = JS_Eval(
            js_runtime->ctx,
            moe_string_get(script),
            script->length,
            script_path,
            JS_EVAL_TYPE_GLOBAL);
    moe_string_free(script);

    if (JS_IsException(ret)) {
        JSValue exception = JS_GetException(js_runtime->ctx);
        const char* error_message = JS_ToCString(js_runtime->ctx, exception);
        printf("Error: Exception when evaluating proc macro script %s: %s\n", script_path, error_message);
        JS_FreeCString(js_runtime->ctx, error_message);
        JS_FreeValue(js_runtime->ctx, exception);
    }
    JS_FreeValue(js_runtime->ctx, ret);

    return 0;
}

int clear_file(const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (f == NULL) {
        return 1;
    }
    fclose(f);
    return 0;
}

int append_to_output(const char* output_file, const char* content) {
    FILE* f = fopen(output_file, "a");
    if (f == NULL) {
        return 1;
    }

    fputs(content, f);
    fclose(f);
    return 0;
}

moe_string_t* extract_base_filename(const char* path) {
    const char* base = strrchr(path, '/');
    if (base == NULL) {
        base = strrchr(path, '\\');
    }
    if (base == NULL) {
        base = path;
    } else {
        base++;
    }

    return moe_string_alloc(base, 0);
}

void append_include_directive(const char* output_file, const char* header_file) {
    FILE* f = fopen(output_file, "a");
    if (f == NULL) {
        return;
    }

    fprintf(f, "#include \"%s\"\n", header_file);
    fclose(f);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <input_file> <output_file> <script_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    const char* script_file = argv[3];

    if (clear_file(output_file)) {
        printf("Error: Failed to clear output file: %s\n", output_file);
        return EXIT_FAILURE;
    }

    moe_string_t* input_base = extract_base_filename(input_file);
    printf("Input file: %s\n", moe_string_get_const(input_base));
    append_include_directive(output_file, moe_string_get_const(input_base));
    moe_string_free(input_base);

    moe_string_t* input_str = read_file(input_file);
    moe_js_runtime_t* js_runtime = moe_js_runtime_alloc();

    if (load_proc_macro_scripts(js_runtime, script_file)) {
        return EXIT_FAILURE;
    }

    moe_string_t* string_store = moe_string_alloc(NULL, 1024);
    stb_lexer lex;
    stb_c_lexer_init(
            &lex,
            moe_string_get_const(input_str),
            moe_string_get_const(input_str) + input_str->length,
            moe_string_get(string_store),
            1024);

    while (stb_c_lexer_get_token(&lex)) {
        if (lex.token == CLEX_parse_error) {
            printf("\n<<<PARSE ERROR>>>\n");
            break;
        }

        moe_derived_struct_t* derived_struct = parse_derived_struct(&lex);
        if (derived_struct != NULL) {
            printf("Found derived struct: %s\n", moe_string_get(derived_struct->struct_name));
            printf("Metadata:\n");

            moe_derive_struct_metadata_print(&derived_struct->metadata);

            JSValue js_token_array = from_token_stream_to_js_array(
                    js_runtime->ctx,
                    &derived_struct->metadata.token_stream_node);

            moe_string_t* generated =
                    generate_code_for_derived_struct(js_runtime, js_token_array, derived_struct);

            if (generated != NULL) {
                if (append_to_output(output_file, moe_string_get(generated)) != 0) {
                    printf("Error: Failed to append generated code to output file: %s\n", output_file);
                }
                moe_string_free(generated);
            }

            JS_FreeValue(js_runtime->ctx, js_token_array);
            moe_derived_struct_free(derived_struct);
        }
    }

    moe_string_free(string_store);
    moe_js_runtime_free(js_runtime);
    moe_string_free(input_str);

    moe_check_memory_leak();

    return EXIT_SUCCESS;
}
