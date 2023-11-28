content = "<!DOCTYPE html>"
content += "<html lang=\"en\">"
content += "<head>"
content += "<meta charset=\"utf-8\">"
content += "<meta name=\"viewport\" content=\"width=device-width\">"
content += "<title>Index CGI</title>"
content += "</head>"
content += "<body>"
content += "<button onclick=\"window.location.href = '/cgi';\">Home</button>"
content += "<h1>Hello from Python CGI</h1>"
content += "</body>"
content += "</html>"
encoded_content = content.encode('utf-8')
content_length = len(encoded_content)

print("HTTP/1.0 200 OK")
print("Content-Type: text/html; charset=utf-8")
print("Content-Length: %d" % content_length)
print("")
print(content)
