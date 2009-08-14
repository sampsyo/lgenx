#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "genx.h"

#define LGENX_DOCUMENT "genx document"
#define LGENX_LIBRARY "genx"

typedef struct {
    genxWriter writer;
} document;

document *newdoc(lua_State *L) {
    document *doc = (document *)lua_newuserdata(L, sizeof(document));
    
    // set the new writer's identifying metatable
    luaL_getmetatable(L, LGENX_DOCUMENT);
    lua_setmetatable(L, -2);
    
    doc->writer = genxNew(NULL, NULL, NULL);
    return doc;
}

document *checkdoc(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, LGENX_DOCUMENT);
    luaL_argcheck(L, ud != NULL, index, "genx document expected");
    return (document *)ud;
}

FILE **checkfile(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, LUA_FILEHANDLE);
    luaL_argcheck(L, ud != NULL, index, "file handle expected");
    return (FILE **)ud;
}

static int handleError(lua_State *L, genxWriter writer, genxStatus status) {
    if (status)
        luaL_error(L, genxLastErrorMessage(writer));
    return 1;
}



static int lgenx_new(lua_State *L) {
    FILE **fh = checkfile(L, 1);
    
    document *doc = newdoc(L);
    genxStatus s = genxStartDocFile(doc->writer, *fh);
    return handleError(L, doc->writer, s);
}

static int lgenx_comment(lua_State *L) {
    document *doc = checkdoc(L, 1);
    const char *text = luaL_checkstring(L, 2);
    genxStatus s = genxComment(doc->writer, (constUtf8)text);
    return handleError(L, doc->writer, s);
}

static int lgenx_startElement(lua_State *L) {
    document *doc = checkdoc(L, 1);
    const char *name = luaL_checkstring(L, 2);
    genxStatus s = genxStartElementLiteral(doc->writer, NULL, (constUtf8)name);
    return handleError(L, doc->writer, s);
}

static int lgenx_endElement(lua_State *L) {
    document *doc = checkdoc(L, 1);
    genxStatus s = genxEndElement(doc->writer);
    return handleError(L, doc->writer, s);
}

static int lgenx_attribute(lua_State *L) {
    document *doc = checkdoc(L, 1);
    const char *key = luaL_checkstring(L, 2);
    const char *value = luaL_checkstring(L, 3);
    genxStatus s = genxAddAttributeLiteral(doc->writer,
                                           NULL,
                                           (constUtf8)key,
                                           (constUtf8)value);
    return handleError(L, doc->writer, s);
}

static int lgenx_text(lua_State *L) {
    document *doc = checkdoc(L, 1);
    const char *text = luaL_checkstring(L, 2);
    genxStatus s = genxAddText(doc->writer, (constUtf8)text);
    return handleError(L, doc->writer, s);
}

static int lgenx_close(lua_State *L) {
    document *doc = checkdoc(L, 1);
    genxStatus s = genxEndDocument(doc->writer);
    handleError(L, doc->writer, s);
    s = genxDispose(w);
    return handleError(L, doc->writer, s);
}



static const struct luaL_reg lgenx[] = {
    {"new", lgenx_new},
    {NULL, NULL}
};

static const struct luaL_reg lgenx_document[] = {
    {"comment", lgenx_comment},
    {"startElement", lgenx_startElement},
    {"endElement", lgenx_endElement},
    {"attribute", lgenx_attribute},
    {"text", lgenx_text},
    {"close", lgenx_close},
    {"__gc", lgenx_close},
    {NULL, NULL}
};

int luaopen_genx(lua_State *L) {
    /* metatable for document objects */
    luaL_newmetatable(L, LGENX_DOCUMENT);
    /* __index is itself */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    /* add methods to metatable */
    luaL_openlib(L, NULL, lgenx_document, 0);
    
    /* register library */
    luaL_openlib(L, LGENX_LIBRARY, lgenx, 0);
    
    return 1;
}