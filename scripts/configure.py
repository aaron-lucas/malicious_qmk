url = input("Request URL: ")

PATH = 'keyboards/dz60_custom/keymaps/default/request_url.h'
with open(PATH, 'w+') as config_file:
    config_file.write(f'#define REQUEST_URL "{url}"')
