'use strict';

exports.__esModule = true;

var _tail2 = require('lodash/tail');

var _tail3 = _interopRequireDefault(_tail2);

var _isString2 = require('lodash/isString');

var _isString3 = _interopRequireDefault(_isString2);

exports.pushQuery = pushQuery;
exports.pushAdditional = pushAdditional;

var _columncompiler = require('./columncompiler');

var _columncompiler2 = _interopRequireDefault(_columncompiler);

var _tablecompiler = require('./tablecompiler');

var _tablecompiler2 = _interopRequireDefault(_tablecompiler);

var _compiler = require('./compiler');

var _compiler2 = _interopRequireDefault(_compiler);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Push a new query onto the compiled "sequence" stack,
// creating a new formatter, returning the compiler.
function pushQuery(query) {
  if (!query) return;
  if ((0, _isString3.default)(query)) {
    query = { sql: query };
  }
  if (!query.bindings) {
    query.bindings = this.formatter.bindings;
  }
  this.sequence.push(query);

  var builder = void 0;
  if (this instanceof _columncompiler2.default) {
    builder = this.columnBuilder;
  } else if (this instanceof _tablecompiler2.default) {
    builder = this.tableBuilder;
  } else if (this instanceof _compiler2.default) {
    builder = this.builder;
  }

  this.formatter = this.client.formatter(builder);
}

// Used in cases where we need to push some additional column specific statements.
function pushAdditional(fn) {
  var child = new this.constructor(this.client, this.tableCompiler, this.columnBuilder);
  fn.call(child, (0, _tail3.default)(arguments));
  this.sequence.additional = (this.sequence.additional || []).concat(child.sequence);
}