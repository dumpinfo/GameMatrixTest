 

#ifndef C4Expressions_h
#define C4Expressions_h


//# \component	Controller System
//# \prefix		Controller/


#include "C4Scripts.h"


namespace C4
{
	typedef Type	TokenType;
	typedef Type	EvaluatorType;


	enum : TokenType
	{
		kTokenIdentifier		= 'IDNT',
		kTokenBoolean			= 'BOOL',
		kTokenInteger			= 'INTG',
		kTokenFloat				= 'FLOT',
		kTokenString			= 'STRG',
		kTokenLeftParen			= 'LEFT',
		kTokenRightParen		= 'RGHT',
		kTokenLeftBracket		= 'LBRC',
		kTokenRightBracket		= 'RBRC',
		kTokenBitwiseNot		= 'BNOT',
		kTokenMultiply			= 'MULT',
		kTokenDivide			= 'DIVD',
		kTokenModulo			= 'MODU',
		kTokenPlus				= 'PLUS',
		kTokenMinus				= 'MINU',
		kTokenShiftLeft			= 'SLFT',
		kTokenShiftRight		= 'SRGT',
		kTokenLess				= 'LESS',
		kTokenGreater			= 'GRTR',
		kTokenLessEqual			= 'LEQU',
		kTokenGreaterEqual		= 'GEQU',
		kTokenEqual				= 'EQUA',
		kTokenNotEqual			= 'NEQU',
		kTokenBitwiseAnd		= 'BAND',
		kTokenBitwiseXor		= 'BXOR',
		kTokenBitwiseOr			= 'BWOR',
		kTokenDot				= 'DOT '
	};


	enum : EvaluatorType
	{
		kEvaluatorValue			= 'VALU',
		kEvaluatorMember		= 'MEMB',
		kEvaluatorArray			= 'ARAY',
		kEvaluatorBoolean		= 'BOOL',
		kEvaluatorInteger		= 'INTG',
		kEvaluatorFloat			= 'FLOT',
		kEvaluatorString		= 'STRG',
		kEvaluatorNegate		= 'NEGA',
		kEvaluatorInvert		= 'IVRT',
		kEvaluatorMultiply		= 'MULT',
		kEvaluatorDivide		= 'DIVD',
		kEvaluatorModulo		= 'MODU',
		kEvaluatorAdd			= 'ADD ',
		kEvaluatorSubtract		= 'SUB ',
		kEvaluatorAnd			= 'AND ',
		kEvaluatorOr			= 'OR  ',
		kEvaluatorXor			= 'XOR ',
		kEvaluatorShiftLeft		= 'SLFT',
		kEvaluatorShiftRight	= 'SRGT',
		kEvaluatorLess			= 'LESS',
		kEvaluatorGreater		= 'GRTR',
		kEvaluatorLessEqual		= 'LEQU',
		kEvaluatorGreaterEqual	= 'GEQU',
		kEvaluatorEqual			= 'EQUA',
		kEvaluatorNotEqual		= 'NEQU'
	};


	class Token : public ListElement<Token>
	{
		private:

			TokenType		tokenType;

		public:

			Token(TokenType type);
			virtual ~Token();

			TokenType GetTokenType(void) const
			{
				return (tokenType);
			}
	};


	class IdentifierToken final : public Token
	{
		private:
 
			ValueName		identifierString;

		public: 

			IdentifierToken(const char *identifier); 
			~IdentifierToken();

			const char *GetIdentifier(void) const 
			{
				return (identifierString); 
			} 
	};


	class BooleanToken final : public Token 
	{
		private:

			bool		booleanValue;

		public:

			BooleanToken(bool value);
			~BooleanToken();

			bool GetBooleanValue(void) const
			{
				return (booleanValue);
			}
	};


	class IntegerToken final : public Token
	{
		private:

			int32		integerValue;

		public:

			IntegerToken(int32 value);
			~IntegerToken();

			int32 GetIntegerValue(void) const
			{
				return (integerValue);
			}
	};


	class FloatToken final : public Token
	{
		private:

			float		floatValue;

		public:

			FloatToken(float value);
			~FloatToken();

			float GetFloatValue(void) const
			{
				return (floatValue);
			}
	};


	class StringToken final : public Token
	{
		private:

			String<>	stringValue;

		public:

			StringToken(const char *string);
			~StringToken();

			const char *GetStringValue(void) const
			{
				return (stringValue);
			}
	};


	class Evaluator : public Tree<Evaluator>, public Packable, public EngineMemory<ScriptState>
	{
		friend class ExpressionMethod;

		private:

			EvaluatorType		evaluatorType;

			union
			{
				mutable int32	evaluatorIndex;
				mutable int32	superIndex;
			};

			virtual Evaluator *Replicate(void) const = 0;

		protected:

			Evaluator(EvaluatorType type);
			Evaluator(const Evaluator& evaluator);

		public:

			virtual ~Evaluator();

			EvaluatorType GetEvaluatorType(void) const
			{
				return (evaluatorType);
			}

			static Evaluator *Create(Unpacker& data, unsigned_int32 unpackFlags);

			Evaluator *Clone(void) const;

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			virtual const Value *Evaluate(const ScriptState *state) = 0;
	};


	class ValueEvaluator : public Evaluator
	{
		friend class Evaluator;

		private:

			ValueName	valueName;

			ValueEvaluator();

			Evaluator *Replicate(void) const override;

		protected:

			ValueEvaluator(EvaluatorType type);
			ValueEvaluator(EvaluatorType type, const char *name);
			ValueEvaluator(const ValueEvaluator& valueEvaluator);

		public:

			ValueEvaluator(const char *name);
			~ValueEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class MemberEvaluator final : public ValueEvaluator
	{
		friend class Evaluator;

		private:

			FloatValue		memberValue;

			MemberEvaluator();
			MemberEvaluator(const MemberEvaluator& memberEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			MemberEvaluator(const char *name);
			~MemberEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class ArrayEvaluator final : public Evaluator
	{
		friend class Evaluator;

		private:

			ValueName		valueName;

			ArrayEvaluator();
			ArrayEvaluator(const ArrayEvaluator& arrayEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			ArrayEvaluator(const char *name);
			~ArrayEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class BooleanEvaluator final : public Evaluator
	{
		friend class Evaluator;

		private:

			BooleanValue	booleanValue;

			BooleanEvaluator();
			BooleanEvaluator(const BooleanEvaluator& booleanEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			BooleanEvaluator(bool value);
			~BooleanEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class IntegerEvaluator final : public Evaluator
	{
		friend class Evaluator;

		private:

			IntegerValue	integerValue;

			IntegerEvaluator();
			IntegerEvaluator(const IntegerEvaluator& integerEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			IntegerEvaluator(int32 value);
			~IntegerEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class FloatEvaluator final : public Evaluator
	{
		friend class Evaluator;

		private:

			FloatValue		floatValue;

			FloatEvaluator();
			FloatEvaluator(const FloatEvaluator& floatEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			FloatEvaluator(float value);
			~FloatEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class StringEvaluator final : public Evaluator
	{
		friend class Evaluator;

		private:

			StringValue		stringValue;

			StringEvaluator();
			StringEvaluator(const StringEvaluator& stringEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			StringEvaluator(const char *value);
			~StringEvaluator();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			const Value *Evaluate(const ScriptState *state) override;
	};


	class OperatorEvaluator : public Evaluator
	{
		private:

			Value	*outputValue;

		protected:

			OperatorEvaluator(EvaluatorType type);
			OperatorEvaluator(const OperatorEvaluator& operatorEvaluator);

			const Value *OutputBooleanValue(bool v);
			const Value *OutputIntegerValue(int32 v);
			const Value *OutputFloatValue(float v);
			const Value *OutputStringValue(const char *v);
			const Value *OutputColorValue(const ColorRGBA& v);
			const Value *OutputVectorValue(const Vector3D& v);
			const Value *OutputPointValue(const Point3D& v);

		public:

			~OperatorEvaluator();
	};


	class NegateEvaluator final : public OperatorEvaluator
	{
		private:

			NegateEvaluator(const NegateEvaluator& negateEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			NegateEvaluator();
			~NegateEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class InvertEvaluator final : public OperatorEvaluator
	{
		private:

			InvertEvaluator(const InvertEvaluator& invertEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			InvertEvaluator();
			~InvertEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class MultiplyEvaluator final : public OperatorEvaluator
	{
		private:

			MultiplyEvaluator(const MultiplyEvaluator& multiplyEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			MultiplyEvaluator();
			~MultiplyEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class DivideEvaluator final : public OperatorEvaluator
	{
		private:

			DivideEvaluator(const DivideEvaluator& divideEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			DivideEvaluator();
			~DivideEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class ModuloEvaluator final : public OperatorEvaluator
	{
		private:

			ModuloEvaluator(const ModuloEvaluator& moduloEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			ModuloEvaluator();
			~ModuloEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class AddEvaluator final : public OperatorEvaluator
	{
		private:

			AddEvaluator(const AddEvaluator& addEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			AddEvaluator();
			~AddEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class SubtractEvaluator final : public OperatorEvaluator
	{
		private:

			SubtractEvaluator(const SubtractEvaluator& subtractEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			SubtractEvaluator();
			~SubtractEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class AndEvaluator final : public OperatorEvaluator
	{
		private:

			AndEvaluator(const AndEvaluator& andEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			AndEvaluator();
			~AndEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class OrEvaluator final : public OperatorEvaluator
	{
		private:

			OrEvaluator(const OrEvaluator& orEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			OrEvaluator();
			~OrEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class XorEvaluator final : public OperatorEvaluator
	{
		private:

			XorEvaluator(const XorEvaluator& xorEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			XorEvaluator();
			~XorEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class ShiftLeftEvaluator final : public OperatorEvaluator
	{
		private:

			ShiftLeftEvaluator(const ShiftLeftEvaluator& shiftLeftEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			ShiftLeftEvaluator();
			~ShiftLeftEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class ShiftRightEvaluator final : public OperatorEvaluator
	{
		private:

			ShiftRightEvaluator(const ShiftRightEvaluator& shiftRightEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			ShiftRightEvaluator();
			~ShiftRightEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class LessEvaluator final : public OperatorEvaluator
	{
		private:

			LessEvaluator(const LessEvaluator& lessEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			LessEvaluator();
			~LessEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class GreaterEvaluator final : public OperatorEvaluator
	{
		private:

			GreaterEvaluator(const GreaterEvaluator& greaterEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			GreaterEvaluator();
			~GreaterEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class LessEqualEvaluator final : public OperatorEvaluator
	{
		private:

			LessEqualEvaluator(const LessEqualEvaluator& lessEqualEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			LessEqualEvaluator();
			~LessEqualEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class GreaterEqualEvaluator final : public OperatorEvaluator
	{
		private:

			GreaterEqualEvaluator(const GreaterEqualEvaluator& greaterEqualEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			GreaterEqualEvaluator();
			~GreaterEqualEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class EqualEvaluator final : public OperatorEvaluator
	{
		private:

			EqualEvaluator(const EqualEvaluator& equalEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			EqualEvaluator();
			~EqualEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class NotEqualEvaluator final : public OperatorEvaluator
	{
		private:

			NotEqualEvaluator(const NotEqualEvaluator& notEqualEvaluator);

			Evaluator *Replicate(void) const override;

		public:

			NotEqualEvaluator();
			~NotEqualEvaluator();

			const Value *Evaluate(const ScriptState *state) override;
	};


	class ExpressionMethod final : public Method
	{
		private:

			Evaluator		*evaluatorRoot;
			String<>		expressionText;

			ExpressionMethod(const ExpressionMethod& expressionMethod);

			Method *Replicate(void) const override;

			static bool TokenizeText(const char *text, List<Token> *tokenList);

			static Evaluator *ParsePrimaryExpression(const Token *& token);
			static Evaluator *ParseUnaryExpression(const Token *& token);
			static Evaluator *ParseMultiplicativeExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseAdditiveExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseShiftExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseRelationalExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseEqualityExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseAndExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseXorExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);
			static Evaluator *ParseOrExpression(const Token *& token, Evaluator *leftEvaluator = nullptr);

			static Evaluator *ParseExpression(const Token *& token)
			{
				return (ParseOrExpression(token));
			}

		public:

			C4API ExpressionMethod();
			C4API ~ExpressionMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			const Evaluator *GetEvaluatorRoot(void) const
			{
				return (evaluatorRoot);
			}

			const char *GetExpressionText(void) const
			{
				return (expressionText);
			}

			C4API void SetExpressionText(const char *text);

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
