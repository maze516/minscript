prep /INC msvcrt /om /ft minscript minipdll.dll
profile minscript -f=xx.ic -r >_x
prep /M minscript
plist minscript >minscript4.out
msdev minscript4.out