import os

print('Content-Type: text/html')
print('')
print('<h1>Environment Variables</h1>')
env = os.environ
for k in env.keys():
    print('<b>%s</b>: %s<br>' % (k, env[k]))