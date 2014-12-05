var http = require('http')
var url = require('url')
var pg = require('pg');
var conString = "postgres://phi:twodice=42@localhost/phi";


function level_save () {
	return {
	}
}

function level_list (time) {
	pg.connect(conString, function(err, client, done) {
	  if(err) {
		  console.error('error fetching client from pool', err);
		  return [];
	  }
	  
	  client.query('SELECT * FROM levels', function(err, result) {
		  
		  done();
		  var ret = [];
		  if(err) {
			  console.error('error running query', err);
			  return [];
		  }
		  else {
			  for (index = 0; index < result.rows.length; ++index) {
				  ret.push(result.row[0]);
				  return ret;
			  }		  	
		  }
	  });
	});
  console.log("test");
	
}

var server = http.createServer(function (req, res) {
  var parsedUrl = url.parse(req.url, true)
  var time = new Date(parsedUrl.query.iso)
  var result

  if (/^\/levels\/list/.test(req.url))
    result = level_list()
  else if (/^\/levels\/save/.test(req.url))
    result = level_save()

  if (result) {
    res.writeHead(200, { 'Content-Type': 'application/json' })
    res.end(JSON.stringify(result))
  } else {
    res.writeHead(404)
    res.end()
  }
})
server.listen(Number(process.argv[2]))