# CostCat-Server

This, is the C++ remake of the CostCat server. The first full API
server built with the initial C++ infrastructure of my re-imagined
tech stack for [nathcat.net](https://nathcat.net).

## Using the server

### Dependencies

- MySql C++ connector

### Building and installation

The following sequence of commands should build and install the server.

```
cmake -B build
cmake --build build
sudo cmake --install build
```

### Configuration

The server requires a few configuration files:

```
Assets/
  authcat_conf.json
  costcat_conf.json
```

#### `authcat_conf.json`

```json
{
  "hostUrl": "<The domain hosting authcat>"
}
```

#### `costcat_conf.json`

```json
{
  "dbUrl": "<URL to the MySQL DB>",
  "dbUsername": "<Username to login to the DB with",
  "dbPassword": "<Password to login to the DB with"
}
```

### Running the server

Once the configuration files are in place, simply run

```
costcat
```

To start the server.

# API Documentation

The following rules apply to _all_ endpoints:

- If authentication fails, a `HTTP 403` error is returned.
- If a different, unspecified error occurs, a `HTTP 500` error is returned, with
  the error detailed as `text/plain` in the response body.

## `POST /transactions/new?group=<group_id>`

Logs a new transaction to the specified group. The `payer` is automatically chosen as the authenticated
user.

### Body format

```json
{
  "amount": 0, // The value transferred in the transaction
  "payees": [], // List of payee user IDs
  "description": "..." // Description of the transaction
}
```

### Response

If successful, a `JSON` object is returned with `HTTP 200`:

```json
{
  "status": "success"
}
```

## `GET /balances?group=<group_id>`

Get the current total balances of the users in a group.

### Response body format

A `JSON` array containing objects of the following format:

```json
{
  "user": 0 // The user's ID,
  "amount": 0 // The user's balance
}
```

Note that in the `amount` field, a negative value indicates that the user owes more than they _are_ owed, i.e. that they are in debt, not credit.

## `GET /debts?group=<group_id>`

Get the smallest set of debts required to achieve equilibrium in the group's current balance.

### Response body format

A `JSON` array containing objects of the following format:

```json
{
  "creditor": 0 // The ID of the creditor
  "debtor": 0 // The ID of the debtor
  "amount": 0 // The amount which must be paid.
}
```

Each object implies a transaction which must take place to erase a debt.
They instruct that the `debtor`, is to pay the `creditor`, `amount`.
