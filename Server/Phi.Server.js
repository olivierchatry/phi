var http = require('http')
var url = require('url')
var pg = require('pg');
var conString = "postgres://phi:twodice=42@localhost/phi";


function level_save () {
	return {
	}
}
function request_error(res)
{
	res.writeHead(404)
    res.end()
}
var level_list = function (res) {
	pg.connect(conString, function(err, client, done) 
	{
		if(err) 
		{
			console.error('error fetching client from pool', err);
			request_error(res);
		}
		else 
		{
			client.query('SELECT * FROM levels', function(err, result) 
			{
				if(err) 
				{
					console.error('error running query', err);
					request_error(res);
				}
				else 
				{
					var ret = [];
					for (index = 0; index < result.rows.length; ++index)
					{
						ret.push(result.row[index]);
					}
					res.writeHead(200, { 'Content-Type': 'application/json' })
					res.end(JSON.stringify(ret))
				}
				done();
			});	
		}	  
	});	
}

var server = http.createServer(function (req, res) {
	var parsedUrl = url.parse(req.url, true);
	var time = new Date(parsedUrl.query.iso);
	var result;

	if (/^\/levels\/list/.test(req.url))
		level_list(res);
	else 
		if (/^\/levels\/save/.test(req.url))
			level_save(res);

});
server.listen(Number(process.argv[2]));