<?PHP

if(empty($conf) or empty($conf['sql']))
	die('You have not yet configured SQL. Cannot continue.');

$dbh = new PDO($conf['sql']['dsn'], $conf['sql']['user'], $conf['sql']['pass']);

# TODO: Change to SILENT if your environment outputs errors
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

function simulate_query($query, $params)
{
	$keys = array();
	$values = array();

	foreach ($params as $key=>$value)
	{
		if (is_string($key))
			$keys[] = '/:'.$key.'/';
		else
			$keys[] = '/\?/';

		if(is_numeric($value))
			$values[] = intval($value);
		else
			$values[] = '"'.$value .'"';
	}

	$query = preg_replace($keys, $values, $query, 1, $count);
	return $query;
}

function hash_password($login, $pass, $salt, $rounds)
{
	$login = strtolower($login);

	$last_hash = hash('sha256', $pass.$login.$salt);

	for($i = 0; $i < $rounds; $i++)
		$last_hash = hash('sha256', $last_hash);

	return $last_hash;
}

function verify_login($login, $pass)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `h`.`user_id`, `h`.`salt`, `h`.`rounds`
		FROM `users` AS `u`
		LEFT JOIN `hashes` AS `h`
			ON `h`.`user_id` = `u`.`id`
		WHERE `u`.`login` = ?');
	$sth->execute(array($login));
	$hash = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	if(empty($hash) or empty($hash['user_id']))
		return false;

	$test_hash = hash_password($login, $pass, $hash['salt'], $hash['rounds']);

	$sth = $dbh->prepare('SELECT `user_id` FROM `hashes`
		WHERE `user_id` = ? AND `hash` = ?');
	$sth->execute(array($hash['user_id'], $test_hash));
	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	if(empty($row) or empty($row['user_id']))
		return false;

	return true;
}

function fetch_pw_reset($token)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `t`.`user_id`, `u`.`login`, `t`.`generated` FROM `pw_tokens` AS `t`
		LEFT JOIN `users` AS `u`
			ON `u`.`id` = `t`.`user_id`
		WHERE `token` = ?');
	$sth->execute(array($token));

	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	return $row;
}

function store_pw_hash($user_id, $hash, $salt, $rounds)
{
	global $dbh;

	$sth = $dbh->prepare('INSERT INTO `hashes` (`user_id`, `hash`, `salt`, `rounds`)
		VALUES (:id, :hash, :salt, :rounds)
		ON DUPLICATE KEY UPDATE
			`hash` = :hash,
			`salt` = :salt,
			`rounds` = :rounds');

	$sth->execute(array(
		'id' => $user_id,
		'hash' => $hash,
		'salt' => $salt,
		'rounds' => $rounds)
	);

	$count = $sth->rowCount();
	$sth->closecursor();

	return ($count > 0);
}

function delete_pw_reset($token)
{
	global $dbh;

	$sth = $dbh->prepare('DELETE FROM `pw_tokens` WHERE `token` = ?');
	$sth->execute(array($token));
	$count = $sth->rowCount();
	$sth->closecursor();

	return ($count > 0);
}

function fetch_client_pubkey($login)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `k`.`pubkey`, `k`.`generated` FROM `users` AS `u`
		LEFT JOIN `client_keys` AS `k`
			ON `k`.`owner_id` = `u`.`id`
		WHERE `u`.`login` = ?');
	$sth->execute(array($login));

	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	return $row;
}

function fetch_client_privkey($login)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `k`.`privkey`, `k`.`generated` FROM `users` AS `u`
		LEFT JOIN `client_keys` AS `k`
			ON `k`.`owner_id` = `u`.`id`
		WHERE `u`.`login` = ?');
	$sth->execute(array($login));

	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	return $row;
}

function fetch_server_pubkey($sname)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `pubkey`, `generated` FROM `server_keys`
		WHERE `server_name` = ?');
	$sth->execute(array($sname));

	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	return $row;
}

function fetch_server_privkey($sname, $secret)
{
	global $dbh;

	$sth = $dbh->prepare('SELECT `privkey`, `generated` FROM `server_keys`
		WHERE `server_name` = ? AND `token` = ?');
	$sth->execute(array($sname, $secret));

	$row = $sth->fetch(PDO::FETCH_ASSOC);
	$sth->closecursor();

	return $row;
}
?>
