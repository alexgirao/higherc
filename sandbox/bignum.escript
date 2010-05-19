#!/usr/bin/env escript
%% -*- erlang -*-

%% ./bignum.escript dc_verify | dc

mul(L=[_|_]) ->
    mul(L, []).

mul([H|T], Acc) ->
    mul(T, [mul2(H, [H | T]) | Acc]);

mul([], Acc) ->
    lists:reverse(Acc).

%%

mul2(A, [H|T]) ->
    [{A, H, A * H} | mul2(A, T)];

mul2(_, []) ->
    [].

%%

-define(bignum_test, true).

-ifdef(bignum_test).

get_L() ->
    A1 = 16#86f7e437faa5a7fce15d1ddcb9eaeaea377667b8,
    B1 = 16#e9d71f5ee7c92d6dc9e92ffdad17b8bd49418f98,
    A2 = 16#abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5,
    B2 = 16#c681e18b81edaf2b66dd22376734dba5992e362bc3f91ab225854c17,
    A3 = 16#ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb,
    B3 = 16#3e23e8160039594a33894f6564e1b1348bbd7a0088d42c4acb73eeaed59c009d,
    A4 = 16#54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31,
    B4 = 16#98a906182cdcfb1eb4eb47117600f68958e2ddd140248b47984f4bde6587b89c8215c3da895a336e94ad1aca39015c40,
    A5 = 16#1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75,
    B5 = 16#5267768822ee624d48fce15ec5ca79cbd602cb7f4c2157a516556991f22ef8c7b5ef7b18d1ff41c59370efb0858651d44a936c11b7b144c48fe04df3c6a3e8da,
    [A1, B1, A2, B2, A3, B3, A4, B4, A5, B5].

-else.

get_L() ->
    A1 = 1,
    A2 = 2,
    A3 = 3,
    [A1, A2, A3].

-endif.

main(Args) ->

    L = get_L(),

    M = mul(L),
    N = lists:map(fun (X=[{A,_,_} | _T]) -> {
			A, length(X), lists:map(fun ({_, B, C}) -> {B, C} end, X)
		       }
		  end, M),
    
    case Args of
	["dc_verify"] ->
	    io:format("[[fail]p q]sa~n"),
	    lists:foreach(fun ({I, _K, T}) ->
				  %io:format("serie ~p has ~p operations~n", [I, K]),
				  lists:foreach(fun ({J, R}) ->
							io:format("~.10.0b ~.10.0b * ~.10.0b !=a~n", [I, J, R]),
							if
							    I /= J ->
								io:format("~.10.0b ~.10.0b * ~.10.0b !=a~n", [J, I, R]);
							    true ->
								ok
							end
						end, T)
			  end, N),
	    io:format("[ok]p~n");
	["c_verify"] ->
	    lists:foreach(fun ({I, _K, T}) ->
				  lists:foreach(fun ({J, R}) ->
							io:format("~.16.0b * ~.16.0b = ~.16.0b~n", [I, J, R])
						end, T)
			  end, N);
	[] ->
	    io:format("valid arguments: dc_verify c_verify~n");
	Else ->
	    io:format("invalid argument ~p~n", [Else])
    end,

    ok.
