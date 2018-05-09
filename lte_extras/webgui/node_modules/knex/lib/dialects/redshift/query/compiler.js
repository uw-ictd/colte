'use strict';

exports.__esModule = true;

var _reduce2 = require('lodash/reduce');

var _reduce3 = _interopRequireDefault(_reduce2);

var _assign2 = require('lodash/assign');

var _assign3 = _interopRequireDefault(_assign2);

var _inherits = require('inherits');

var _inherits2 = _interopRequireDefault(_inherits);

var _compiler = require('../../../query/compiler');

var _compiler2 = _interopRequireDefault(_compiler);

var _compiler3 = require('../../postgres/query/compiler');

var _compiler4 = _interopRequireDefault(_compiler3);

var _helpers = require('../../../helpers');

var helpers = _interopRequireWildcard(_helpers);

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

// Redshift Query Builder & Compiler
// ------
function QueryCompiler_Redshift(client, builder) {
  _compiler4.default.call(this, client, builder);
}
(0, _inherits2.default)(QueryCompiler_Redshift, _compiler4.default);

(0, _assign3.default)(QueryCompiler_Redshift.prototype, {
  truncate: function truncate() {
    return 'truncate ' + this.tableName.toLowerCase();
  },


  // Compiles an `insert` query, allowing for multiple
  // inserts using a single query statement.
  insert: function insert() {
    var sql = _compiler2.default.prototype.insert.apply(this, arguments);
    if (sql === '') return sql;
    this._slightReturn();
    return {
      sql: sql
    };
  },


  // Compiles an `update` query, warning on unsupported returning
  update: function update() {
    var sql = _compiler2.default.prototype.update.apply(this, arguments);
    this._slightReturn();
    return {
      sql: sql
    };
  },


  // Compiles an `delete` query, warning on unsupported returning
  del: function del() {
    var sql = _compiler2.default.prototype.del.apply(this, arguments);
    this._slightReturn();
    return {
      sql: sql
    };
  },


  // simple: if trying to return, warn
  _slightReturn: function _slightReturn() {
    if (this.single.isReturning) {
      helpers.warn('insert/update/delete returning is not supported by redshift dialect');
    }
  },
  forUpdate: function forUpdate() {
    helpers.warn('table lock is not supported by redshift dialect');
    return '';
  },
  forShare: function forShare() {
    helpers.warn('lock for share is not supported by redshift dialect');
    return '';
  },


  // Compiles a columnInfo query
  columnInfo: function columnInfo() {
    var column = this.single.columnInfo;

    var sql = 'select * from information_schema.columns where table_name = ? and table_catalog = ?';
    var bindings = [this.single.table.toLowerCase(), this.client.database().toLowerCase()];

    if (this.single.schema) {
      sql += ' and table_schema = ?';
      bindings.push(this.single.schema);
    } else {
      sql += ' and table_schema = current_schema()';
    }

    return {
      sql: sql,
      bindings: bindings,
      output: function output(resp) {
        var out = (0, _reduce3.default)(resp.rows, function (columns, val) {
          columns[val.column_name] = {
            type: val.data_type,
            maxLength: val.character_maximum_length,
            nullable: val.is_nullable === 'YES',
            defaultValue: val.column_default
          };
          return columns;
        }, {});
        return column && out[column] || out;
      }
    };
  }
});

exports.default = QueryCompiler_Redshift;
module.exports = exports['default'];