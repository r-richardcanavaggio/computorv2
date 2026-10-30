#pragma once

#include "RuntimeTypes.hpp"

VarType	applyUnaryOp( const OpKind&, const VarType& );
VarType	applyBinaryOp( const OpKind&, const VarType&, const VarType& );
