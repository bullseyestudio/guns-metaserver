<?PHP

if(empty($conf) or empty($conf['sql']))
	die('You have not yet configured SQL. Cannot continue.');

$dbh = new PDO($conf['sql']['dsn'], $conf['sql']['user'], $conf['sql']['pass']);

# TODO: Change to SILENT if your environment outputs errors
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

?>
