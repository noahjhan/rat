# Process to Introduce New Tokens

## If Token Already Exists

1. **Add new representation as string in set in lexer constructor**  
   Add the new token representation in the appropriate set in the lexer constructor.

2. **Check `isAcceptable` function for the following:**
   - **Conflicts with current implementation**:  
     If the token conflicts with an existing one, it will cause ambiguous tokens.
   - **Uninclusive implementation**:  
     If the token is not recognized, it will be an unrecognized token.

3. **Add `ConstituentToken` and translation in `token.hpp` and `dictionary.hpp`:**  
   Update the token and dictionary headers to reflect the new constituent token and its translation.

## If Token is New

1. **Add new set in lexer constructor and lexer class**  
   Include the new set in both the lexer constructor and class.

2. **Add new token as `GenericToken` in `token.hpp`:**  
   Declare the new token as a `GenericToken` in the `token.hpp` file.

3. **Create new `isAcceptable` function:**  
   Write a new `isAcceptable` function to handle the new token’s specific acceptance criteria.

4. **Modify `advanceToken` function to avoid ambiguous tokens:**  
   Ensure that the new token cannot be interpreted in more than one way. This ensures the lexer doesn't misinterpret the token.

5. **Add `ConstituentToken` and translation in `token.hpp` and `dictionary.hpp`:**  
   Similarly to when a token already exists, ensure the new token's constituent and translation are added to the respective headers.

6. **Add `error_checker` in `parser.hpp`:**  
   Introduce an `error_checker` to help detect errors related to the new token in the parser.
