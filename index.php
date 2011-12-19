<?PHP
$_ARGS = array();
if(!empty($_GET['args']))
{
	$_ARGS = explode('/', $_GET['args']);
}

include('inc/config.inc');
?>
