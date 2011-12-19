<?PHP
# Formatting functions.

function fmt_duration($secs, $short = false, $drop_secs = false)
{
	if(empty($secs) or ($drop_secs and $secs < 60))
		return 'just now';

	$suffix = ' ago';
	if($secs < 0)
	{
		$suffix = ' from now';
		$secs = abs($secs);
	}

	$mins = floor($secs / 60);
	$hrs = floor($mins / 60);

	$secs = $secs % 60;
	$mins = $mins % 60;

	$str = '';

	if(!empty($hrs))
	{
		$str .= $hrs.($short ? 'h' : ' hour').($hrs != 1 ? 's' : '');
	}

	if(!empty($mins))
	{
		if(!empty($hrs))
			$str .= ($short ? ' ' : ', ');

		$str .= $mins.($short ? 'm' : ' minute').($mins != 1 ? 's' : '');
	}

	if(!empty($secs) and !$drop_secs)
	{
		if(!empty($hrs) or !empty($mins))
			$str .= ($short ? ' ' : ', ');

		$str .= $secs.($short ? 's' : ' second').($secs != 1 ? 's' : '');
	}

	return $str.$suffix;
}

?>
