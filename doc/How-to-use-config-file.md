# How to use config file

To configure the server, you need to create a config file. The config file is a text file with the `.lms` extension (Loïc, Mathias, Sylvain).

It contains `directives`, `sub-directives` and `options`.

## Directive
Directive contains sub-directives and options.

- Directive is defined by a name followed by a colon `:`.
- To put sub-directives or options in a directive, indent them with a tabulation.
- To close a directive, use a semicolon `;`.

Allowing directives: [server].

### Example

```
directive:
	option="value"
	option="value"
;
```

## Sub-directive
Sub-directive contains options.

- Sub-directive is defined by a name followed by a colon `:`.
- To put options in a sub-directive, indent them with a tabulation.
- To close a sub-directive, use a semicolon `;`.

Allowing sub-directives: [route].

### Example

```
directive:
	option="value"
	option="value"
	sub-directive:
		option="value"
	;
;
```

## Option
Option is a key-value pair.

- Option is defined by a key followed by an equal sign `=` and a value surrounded by double quotes `"`.
- To close an option, use a line break.
- Spaces are not allowed.

### Example

```
option="value"
```

## Usage

### Server

#### Sub-directives
- `route`: Route configuration.
  - Default: `none`

#### Options
- `name`: The name of the server.
  - Default: `webserv`
- `host`: The host of the server.
  - Default: `localhost`
- `port`: The port of the server.
  - Default: `8080`
- `client_max_body_size`: The maximum size of the body of a request.
  - Default: `50M`
- `error_page_400`: Set the path to the 400 error page.
  - Default: `www/error_pages/400.html`
- `error_page_403`: Set the path to the 403 error page.
  - Default: `www/error_pages/403.html`
- `error_page_404`: Set the path to the 404 error page.
  - Default: `www/error_pages/404.html`
- `error_page_405`: Set the path to the 405 error page.
  - Default: `www/error_pages/405.html`
- `error_page_413`: Set the path to the 413 error page.
  - Default: `www/error_pages/413.html`
- `error_page_500`: Set the path to the 500 error page.
  - Default: `www/error_pages/500.html`

### Route

#### Options
- `uri`: The URI of the route.
  - Default: `/`
- `path`: The path to the resource.
  - Default: `www/example`
- `index`: The index of the route.
  - Default: `index.html`
- `get`: Enable or disable the GET method.
  - Default: `true`
- `post`: Enable or disable the POST method.
  - Default: `false`
- `delete`: Enable or disable the DELETE method.
  - Default: `false`
- `autoindex`: Enable or disable the autoindex (list ressources of the directory).
  - Default: `false`
- `cgi`: Path to the CGI executable.
  - Default: `none`
