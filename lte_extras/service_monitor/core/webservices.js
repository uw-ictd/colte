const exec = require('child_process').exec;
const readline = require('readline');
//const fs = require('fs');

//const rl = readline.createInterface({
//  input: fs.createReadStream(__dirname + '/command.txt')
//});

const webservices = module.exports;

webservices.status = () => new Promise((resolve, reject) => {
	var promises = [];

  var lines = require('fs').readFileSync(__dirname + '/command.txt', 'utf-8').split('\n');
  lines.forEach( (line) => {
		var title = line.substr(0, line.indexOf('='));
		var command = line.substr(line.indexOf('=') + 1);
		var promise = promisifyExec(title, command);
		promises.push(promise);
		//console.log(title + '_' + command);
	});

  Promise.all(promises).then(function(values) {
    var result = new Object();
    values.forEach( (element) => {
      result[element.title] = parseInt(element.stdout);
    });
    console.log(result);
    resolve(result);
  });
});

function promisifyExec(title, command) {
  return new Promise((resolve, reject) => {
    exec(command, (error, stdout, stderr) => {
      if (error) {
				reject(error);
      } else {
				resolve({title, stdout});
      }
    });
  });
}

//function readCommands() {

