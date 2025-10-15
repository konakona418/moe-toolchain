// Procedural macro for generating debug print functions for structs

globalThis.debug = function(name, tokens) {
    function indent(n) {
        return '    '.repeat(n);
    }

    let fields = [];
    for (let i = 0; i < tokens.length; ++i) {
        let tok = tokens[i];
        if (tok.type === 'moe_token_type_identifier') {
            let typeName = tok.value;
            if (typeName === 'int' || typeName === 'float' || typeName === 'double' || typeName === 'struct' || typeName === 'enum') {
                // handle struct case
                let nextType = typeName;
                let structName = null;

                if (typeName === 'struct') {
                    let next = tokens[i + 1];
                    if (next && next.type === 'moe_token_type_identifier') {
                        structName = next.value;
                        nextType = 'struct ' + structName;
                        i++;
                    }
                }

                if (typeName === 'enum') {
                    let next = tokens[i + 1];
                    if (next && next.type === 'moe_token_type_identifier') {
                        nextType = 'enum ' + next.value;
                        i++;
                    }
                }

                // next should be identifier or pointer star
                let ptrDepth = 0;
                while (tokens[i + 1] && tokens[i + 1].type === 'moe_token_type_punctuation' && tokens[i + 1].value === '*') {
                    ptrDepth++;
                    i++;
                }

                let next = tokens[i + 1];
                if (next && next.type === 'moe_token_type_identifier') {
                    fields.push({type: nextType, name: next.value, ptr: ptrDepth});
                    i++;
                }
            }
        }
    }

    let code = '';
    code += 'void _moe_proc_macro_' + name + '_derive_debug_impl_helper(struct ' + name + ' *self, int base_ident) {\n';
    code += indent(1) + 'if (base_ident == 0) printf("%*s' + name + ' {\\n", base_ident * 4, "");\n';
    code += indent(1) + 'else printf("' + name + ' {\\n");\n';

    for (let f of fields) {
        let base = indent(1);
        let fieldExpr = 'self->' + f.name;

        if (f.ptr > 0) {
            if (f.type === 'int' || f.type === 'float' || f.type === 'double') {
                code += `${base}printf("%*s${f.name}: ${f.type} = %p", (base_ident + 1) * 4, "", (void*)${fieldExpr});\n`;
                code += `${base}if (${fieldExpr}) printf(" -> ${f.type == 'int' ? '%d' : '%f'}", *${fieldExpr});\n`;
                code += `${base}printf("\\n");\n`;
            } else if (f.type.startsWith('struct ')) {
                code += `${base}printf("%*s${f.name}: struct _* (%p) = ", (base_ident + 1) * 4, "", (void*)${fieldExpr});\n`;
                code += `${base}if (${fieldExpr}) _moe_proc_macro_${f.type.replace('struct ', '')}_derive_debug_impl_helper(${fieldExpr}, base_ident + 1);\n`;
            } else if (f.type.startsWith('enum ')) {
                code += `${base}printf("%*s${f.name}: enum ${f.type.replace('enum ', '')}* (%p) = ", (base_ident + 1) * 4, "", (void*)${fieldExpr});\n`;
                code += `${base}if (${fieldExpr}) _moe_proc_macro_${f.type.replace('enum ', '')}_derive_debug_enum_impl(*${fieldExpr});\n`;
                code += `${base}printf("\\n");\n`;
            } else {
                code += `${base}printf("%*s${f.name}: ${f.type} = %p\\n", (base_ident + 1) * 4, "", (void*)${fieldExpr});\n`;
            }
        } else {
            if (f.type === 'int') {
                code += `${base}printf("%*s${f.name}: ${f.type} = %d\\n", (base_ident + 1) * 4, "", ${fieldExpr});\n`;
            } else if (f.type === 'float' || f.type === 'double') {
                code += `${base}printf("%*s${f.name}: ${f.type} = %f\\n", (base_ident + 1) * 4, "", ${fieldExpr});\n`;
            } else if (f.type.startsWith('struct ')) {
                let subStruct = f.type.replace('struct ', '');
                code += `${base}printf("%*s${f.name}: struct _ = ", (base_ident + 1) * 4, "");\n`;
                code += `${base}_moe_proc_macro_${subStruct}_derive_debug_impl_helper(&${fieldExpr}, base_ident + 1);\n`;
            } else if (f.type.startsWith('enum ')) {
                let subEnum = f.type.replace('enum ', '');
                code += `${base}printf("%*s${f.name}: enum ${subEnum} = ", (base_ident + 1) * 4, "");\n`;
                code += `${base}_moe_proc_macro_${subEnum}_derive_debug_enum_impl(${fieldExpr});\n`;
                code += `${base}printf("\\n");\n`;
            } else {
                code += `${base}printf("%*s${f.name}: ${f.type} = <unknown>\\n", (base_ident + 1) * 4, "");\n`;
            }
        }
    }

    code += indent(1) + 'printf("%*s}\\n", base_ident * 4, "");\n';
    code += '}\n';

    code += 'void _moe_proc_macro_' + name + '_derive_debug_impl(struct ' + name + ' *self) {\n';
    code += indent(1) + `_moe_proc_macro_${name}_derive_debug_impl_helper(self, 0);\n`;
    code += '}\n';

    return code;
};

globalThis.debug_enum = function(name, tokens) {
    function indent(n) {
        return '    '.repeat(n);
    }

    let variants = [];
    for (let i = 0; i < tokens.length; ++i) {
        if (tokens[i].type === 'moe_token_type_identifier') {
            variants.push(tokens[i].value);
        }
    }

    let code = '';
    code += 'void _moe_proc_macro_' + name + '_derive_debug_enum_impl(enum ' + name + ' value) {\n';
    code += indent(1) + 'switch(value) {\n';
    for (let i = 0; i < variants.length; ++i) {
        code += indent(2) + 'case ' + variants[i] + ':\n';
        code += indent(3) + 'printf("' + variants[i] + '");\n';
        code += indent(3) + 'break;\n';
    }
    code += indent(2) + 'default:\n';
    code += indent(3) + 'printf("<unknown>");\n';
    code += indent(3) + 'break;\n';
    code += indent(1) + '}\n';
    code += '}\n';

    return code;
};
