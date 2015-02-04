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

// URI parsing automaton

// TODO: handle IPv6 addresses in "host" field

%%{
    machine uri_parser;

    action fin_scheme   { set_url_component(&components->scheme,   s, mark.scheme, p); }
    action fin_username { end_username = p; }
    action fin_password { end_password = p; }
    action fin_host     { set_url_component(&components->host,     s, mark.host, p); }
    action fin_port     { set_url_component(&components->port,     s, mark.port, p); }
    action fin_path     { set_url_component(&components->path,     s, mark.path, p); }
    action fin_query    { set_url_component(&components->query,    s, mark.query, p); }
    action fin_fragment { set_url_component(&components->ref,      s, mark.fragment, p); }

    action fin_userpass {
        if (mark.username)
            set_url_component(&components->username, s, mark.username, end_username);
        if (mark.password)
            set_url_component(&components->password, s, mark.password, end_password);
    }

    scheme =    [^:/?#]+        >{mark.scheme = p;}   %fin_scheme;
    username =  [^/?#@:]+       >{mark.username = p; end_username = p;} %fin_username;
    password =  [^/?#@]+        >{mark.password = p; end_password = p;} %fin_password;
    host =      [^/?#@:]+       >{mark.host = p;}     %fin_host;
    port =      digit+          >{mark.port = p;}     %fin_port;
    path =      ([^?#]*)        >{mark.path = p;}     %fin_path;
    query =     ([^#]*)         >{mark.query = p;}    %fin_query;
    fragment =  (any*)          >{mark.fragment = p;} %fin_fragment;

    scheme_c = scheme . ":";
    userpass = (username . (":" . password)? . "@")   %fin_userpass;
    authority = userpass? . host . (":" . port)?;
    slashslash_authority = "//" . authority?;
    qm_query = "?" . query;
    sharp_fragment = "#" . fragment;

    main := scheme_c? . slashslash_authority? . path . qm_query? . sharp_fragment?;

}%%

%%{
    write data;
    write init;
    write exec;
}%%
