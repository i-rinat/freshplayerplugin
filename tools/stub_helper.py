from __future__ import print_function
import sys
import glob

from pycparser import c_parser, c_ast, parse_file, c_generator

c_gen = c_generator.CGenerator()

def lowcase_name(iname, fname):
    s = list(iname.replace("PPB_", "ppb") + fname)
    s2 = []
    for b in s:
        if b.isupper():
            s2.append('_')
            s2.append(b.lower())
        else:
            s2.append(b)
    return "".join(s2).replace("__", "_")

def gen_impl_functions(ast, gen_prototypes_only=False):
    class MyVisitor(c_ast.NodeVisitor):
        def visit_Struct(self, node):
            if node.name is not None and node.name.startswith("PPB_") and node.decls is not None:
                for a in node.decls:
                    a_func = a.children()[0][1].children()[0][1].children()
                    a_func_args = a_func[0][1]
                    a_func_rettype = a_func[1][1]

                    fname = "unknown"
                    if isinstance(a_func_rettype, c_ast.TypeDecl):
                        fname = a_func_rettype.declname
                        a_func_rettype.declname = ""
                        print(c_gen.visit(c_ast.Typedef("dummy", [], [], a_func_rettype)))
                        a_func_rettype.declname = fname
                    elif isinstance(a_func_rettype, c_ast.PtrDecl):
                        a_func_rettype = a_func_rettype.children()[0][1]
                        fname = a_func_rettype.declname
                        a_func_rettype.declname = ""
                        print (c_gen.visit(c_ast.Typedef("dummy", [], [], a_func_rettype)) + " *")
                        a_func_rettype.declname = fname

                    arg_names = (list(c[1].name for c in a_func_args.children() if c[1].name is not None))
                    print(lowcase_name(node.name, fname) + "(" + c_gen.visit(a_func_args) + ")", end="")
                    if gen_prototypes_only:
                        print(";")
                    else:
                        print("")
                        print("{")
                        print("}")
                    print("")

    v = MyVisitor()
    v.visit(ast)


def gen_trace_functions(ast):
    class MyVisitor(c_ast.NodeVisitor):
        def visit_Struct(self, node):
            if node.name is not None and node.name.startswith("PPB_") and node.decls is not None:
                for a in node.decls:
                    a_func = a.children()[0][1].children()[0][1].children()
                    a_func_args = a_func[0][1]
                    a_func_rettype = a_func[1][1]

                    print("TRACE_WRAPPER")
                    fname = "unknown"
                    if isinstance(a_func_rettype, c_ast.TypeDecl):
                        fname = a_func_rettype.declname
                        a_func_rettype.declname = ""
                        print(c_gen.visit(c_ast.Typedef("dummy", [], [], a_func_rettype)))
                        a_func_rettype.declname = fname
                    elif isinstance(a_func_rettype, c_ast.PtrDecl):
                        a_func_rettype = a_func_rettype.children()[0][1]
                        fname = a_func_rettype.declname
                        a_func_rettype.declname = ""
                        print (c_gen.visit(c_ast.Typedef("dummy", [], [], a_func_rettype)) + " *")
                        a_func_rettype.declname = fname

                    arg_names = (list(c[1].name for c in a_func_args.children() if c[1].name is not None))
                    print("trace_" + lowcase_name(node.name, fname) + "(" + c_gen.visit(a_func_args) + ")")
                    print("{")
                    print('    trace_info("[PPB] {zilch} %s\\n", __func__+6);')
                    print("    return " + lowcase_name(node.name, fname) + "(" + ", ".join(arg_names) + ");")
                    print("}")
                    print("")

    v = MyVisitor()
    v.visit(ast)

def gen_structs(ast):
    class MyVisitor(c_ast.NodeVisitor):
        def visit_Struct(self, node):
            if node.name is not None and (node.name.startswith("PPB_") or node.name.startswith("PPP_")) and node.decls is not None:
                print("const struct " + node.name + " " + lowcase_name(node.name, "") + " = {")
                for a in node.decls:
                    a_func = a.children()[0][1].children()[0][1].children()
                    a_func_args = a_func[0][1]
                    a_func_rettype = a_func[1][1]

                    fname = "unknown"
                    if isinstance(a_func_rettype, c_ast.TypeDecl):
                        fname = a_func_rettype.declname
                    elif isinstance(a_func_rettype, c_ast.PtrDecl):
                        a_func_rettype = a_func_rettype.children()[0][1]
                        fname = a_func_rettype.declname

                    print("    ." + fname + " = TWRAPZ(" + lowcase_name(node.name, fname) + "),")
                print("};")

    v = MyVisitor()
    v.visit(ast)

prep_file = sys.argv[1]
ast = parse_file(prep_file, use_cpp=False)

guardian_name = "FPP_" + prep_file.replace(".prep", "").upper().replace(".", "_")
print("#ifndef " + guardian_name)
print("#define " + guardian_name)
print("")
print("// includes here")
print("")
print("")
gen_impl_functions(ast, True)
print("#endif // " + guardian_name)
print("")

print("#include \"" +prep_file.replace(".prep", "") + "\"")
print("#include <stdlib.h>")
print("#include \"trace.h\"")
print("")
print("")

gen_impl_functions(ast)
print("\n// trace wrappers")
gen_trace_functions(ast)
gen_structs(ast)
