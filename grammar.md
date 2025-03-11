$$
\begin{aligned}
\text{Function} &\to \text{Type} \ \text{Identifier} ( \text{Parameters} ) \ \text{Body} \\
\text{Type} &\to \text{int} \ | \ \text{float} \ | \ \text{void} \\
\text{Identifier} &\to \text{[a-zA-Z][a-zA-Z0-9]*} \\
\text{Parameters} &\to \text{Type} \ \text{Identifier} \ ( , \ \text{Type} \ \text{Identifier} )^* \ | \ \varepsilon \\
\text{Body} &\to \{ \text{Statements} \} \\
\text{Statements} &\to \text{Statement} \ (\text{Statement})^* \\
\text{Statement} &\to \text{Expression} ; \ | \ \text{Return} ; \\
\text{Return} &\to \text{return} \ \text{Expression} \\
\text{Expression} &\to \text{Identifier} \ | \text{Literal} 
\end{aligned}
$$
