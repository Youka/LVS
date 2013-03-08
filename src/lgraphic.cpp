#include "llibs.hpp"

int luaopen_graphic(lua_State *L){
	return 0;
}

/*inline LOGFONTW CreateLogfont(int size, bool bold, bool italic, bool underline, bool strikeout, BYTE charset, const wchar_t *wface){
	LOGFONTW lf = {0};
	lf.lfHeight = size;
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = italic;
	lf.lfUnderline = underline;
	lf.lfStrikeOut = strikeout;
	lf.lfCharSet = charset;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfFaceName[31] = L'\0';
	wcsncpy(lf.lfFaceName, wface, 31);
	return lf;
}*/