# Documentation

## Directives
The configuration file consists of directives and their parameters. Simple (single-line) directives each end with a semicolon (`;`). Other directives act as "containers" that group together related directives, enclosing them in curly braces (`{}`); these are often referred to as blocks.

### Simple directives
Simple directives are optional and have default settings when they're not included. The following are the options, followed by an example:

`client_max_body_size <size in bytes>;`

```conf
client_max_body_size 1000000;
```

`error_page <error_id> <path to file>;`

```conf
error_page 404 /404.html;
```

## Contexts

### Servers
A top-level directive, referred to as a context, groups together the directives that apply to different servers. Each single-line directive must end with a semicolon (`;`). These directives include `server_name`, `root`, `index`, `cgi_path`, and the container `location`. Every server must have a single `server_name`, `root`, `index`, and `location`. `cgi_path` is an optional directive, which can appear multiple times, as can the `location` container. A single configuration file can have as many servers as required, as long as each has a unique port.

### Locations
The `location` container has multiple single-line directives. All of the directives are optional. Each directive must end with a semicolon (`;`). If the location has no directives, the files in the given location will be served. Depending on the directives, the location of these files, or what is served, may change. The possible directives are: `listen_http`, `listen_https`, `alias`, `auto_index`, `limit_except`, and `index`. For the `listen_http`/`listen_https` directives specifically, only one of the two may be included. If both are present, the server won't launch. If neither is present, the server will default to `http`.

The following is an example:

```conf
server <port>
{
	server_name <servername>;
	root <path to root>;
	index <index file>;
	cgi_path <cgi file suffix> <path to cgi script>;
	location <path to location>
	{
		alias <path to actual files>;
		auto_index on;
		index <index file>;
		limit_except <HTTP Method>;
	}
}
```

## Comments
You can insert comments into a configuration file in two ways. Everything after a semicolon (`;`) until the line break will not be read. The second option is inserting a pound sign (`#`) at the front of your comment. Everything after it will not be read.

## Example of configuration file
The following is an example of a configuration file. This file is also your default configuration file, stored in `default.conf` under the `default_files` directory.

```conf
client_max_body_size 1000000;
error_page 404 default_files/err_pages/404.html;

server 80
{
	server_name localhost;
	root /var/www/html;
	index index.html;
	cgi_path .cgi /var/run/php/php7.4-fpm.sock;
	
	location / # This will serve files straight from the root directory
	{
		auto_index on;
		listen_https on;
	}

	location /forum
	{
		alias /forumfiles;
		listen_https on;
	}
}
```