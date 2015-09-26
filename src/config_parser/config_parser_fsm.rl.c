/*
 * Copyright © 2013-2015  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

%%{
    machine config_parser;

    action param_name_begin { param.name_start = p; }
    action param_name_end { param.name_end = p; }

    action param_val_begin { param.val_start = p; }
    action param_val_end {
        param.val_end = p;
        handle_param(cfg, param.name_start, param.name_end, param.val_start, param.val_end);
    }

    param_name = ((alpha | '_') (alnum | '_')*)  >param_name_begin %param_name_end;

    param_val = ((any - '\n' - '\r' - "'" - '"')+) >param_val_begin %param_val_end;

    param_val_qt = "'" ((any - "'")* >param_val_begin %param_val_end) "'";
    param_val_dblqt = '"' ((any - '"')* >param_val_begin %param_val_end) '"';

    empty_line = space*;

    comment_line = space* '#' ( any - ';' - '\n' - '\r' )*;

    name_value_line = space* param_name
                      ( space* '=' space* (param_val_qt | param_val_dblqt | param_val) space* )?;

    line = ( name_value_line | empty_line | comment_line );

    main := (line ';'? [\r\n]+)* (line ';'?)?;

}%%

%%{
    write data;
    write init;
    write exec;
}%%
