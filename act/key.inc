<?PHP
$_HEAD = apache_request_headers();

header('Content-Type: text/plain');

$client_key = false;
$private_key = false;
$identifier = null;
$key = null;

if(stripos($_ARGS[1], 'client') !== false)
	$client_key = true;
elseif(stripos($_ARGS[1], 'server') !== false)
	$client_key = false;
else
{
	header('HTTP/1.x 400 Bad Request');
	die('Bad request: first parameter should be either "client" or "server".');
}

if(stripos($_ARGS[2], 'private') !== false)
{
	$private_key = true;
	$identifier = $_ARGS[3];
}
else
	$identifier = $_ARGS[2];

$check_time = 0;
if(!empty($_HEAD['If-Modified-Since']))
	$check_time = strtotime($_HEAD['If-Modified-Since']);

if($client_key and !$private_key)
	$key = fetch_client_pubkey($identifier);
elseif($client_key and $private_key)
{
	if(verify_login($identifier, $_ARGS[4])) #TODO: get password from $_POST['password']?
		$key = fetch_client_privkey($identifier);
	else
	{
		header('HTTP/1.x 401 Unauthorized');
		die('Bad login.');
	}
}
elseif(!$client_key and !$private_key)
	$key = fetch_server_pubkey($identifier);
elseif(!$client_key and $private_key)
{
	$key = fetch_server_privkey($identifier, $_ARGS[4]);
	$identifier .= '/'.$_ARGS[4];
}
else
{
	header('HTTP/1.x 400 Bad Request');
	die('Bad request: You should never see this. It would mean you somehow asked for a key that was neither client nor server, but this is caught earlier in the code.');
}

if(empty($key) or empty($key['generated']))
{
	header('HTTP/1.x 400 Bad Request');
	die('Bad request: No key for "'.$identifier.'".');
}

$key_generated = strtotime($key['generated']);

if($check_time == $key_generated)
{
	header('HTTP/1.x 304 Not Modified');
	die();
}

header('HTTP/1.x 200 OK');
header('Last-Modified: '.date('r', $key_generated));

if($private_key)
	print($key['privkey']);
else
	print($key['pubkey']);

?>
