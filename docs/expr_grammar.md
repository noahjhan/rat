$$
\begin{aligned}
\text{Start} &\mathrel{::=} \text{Expression} \ | \ \varepsilon \\
\text{Expression} &\mathrel{::=} \text{Term} \ \text{HP-OP} \ | \ \text{Term} \\
\text{Term} &\mathrel{::=} \text{Factor} \ \text{LP-OP} \ | \ \text{Factor} \\ 
\text{Factor} &\mathrel{::=} ( \text{Literal} \ | \ \text{Identifier} ) \ | \ \text{``(" Expression ``)"} \\
\text{LP-OP} &\mathrel{::=} ``+" \ | \ ``-" \\
\text{HP-OP} &\mathrel{::=} ``*" \ | \ ``/ \ " \\
\text{Literal} &\mathrel{::=} [0-9]+ \\
\text{Identifier} &\mathrel{::=} \text{Literal} \ | \ \text{Function(Parameter*)} \\
\end{aligned} \\
$$
