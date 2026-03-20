#!/bin/bash

# --- 設定區 ---
LOGIN_PAGE="http://QQQQQQQQQQ/authentication.php"
AUTH_URL="http://QQQQQQQQQQ/authentication.php"
USERNAME="12345678"
PASSWORD="12345678"

# Firefox 120 的 User-Agent
FF_UA="Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Gecko/20100101 Firefox/120.0"


SESSION_ID=$(curl -s --compressed -c cookies.txt -A "$FF_UA" "$LOGIN_PAGE" \
            | grep -a "PHPSESSID" \
            | grep -oP 'value="\K[^"]+')

if [ -z "$SESSION_ID" ]; then
    echo "[失敗] 無法從網頁提取 PHPSESSID，請檢查網址或連線。"
    exit 1
fi

echo "成功提取 SID: $SESSION_ID"

echo "==== 正在模擬 Firefox 登入 ===="

# 執行 POST，帶入剛抓到的 SID
# 這裡也加上 --compressed 以防伺服器回傳壓縮的登入結果
curl -s --compressed -L -A "$FF_UA" -b cookies.txt -e "$LOGIN_PAGE" -X POST \
  -d "PHPSESSID=$SESSION_ID" \
  -d "mac=" \
  -d "debug=" \
  -d "username=$USERNAME" \
  -d "password=$PASSWORD" \
  -d "commit=Log in" \
  "$AUTH_URL" > /dev/null

# 最後驗證
sleep 2
CHECK=$(curl -s -o /dev/null -w "%{http_code}" http://www.google.com/generate_204)

if [ "$CHECK" == "204" ]; then
    echo "==== 恭喜！登入成功，網路已連通 ===="
else
    echo "==== 登入似乎沒成功，請檢查帳密或參考 log ===="
fi