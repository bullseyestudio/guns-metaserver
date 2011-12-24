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

?>
