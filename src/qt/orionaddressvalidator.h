// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ORION_QT_ORIONADDRESSVALIDATOR_H
#define ORION_QT_ORIONADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class OrionAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit OrionAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Orion address widget validator, checks for a valid orion address.
 */
class OrionAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit OrionAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // ORION_QT_ORIONADDRESSVALIDATOR_H
